/*
  QDYN2EXODUS - transfer qdyn native format file into exodusii format.
  So far it only works for 2D faults and based on snapshot qdyn files
*/

#include "QTimer"
#include "QtCore"

#include "exodusII.h"

#include <iostream>
//#include <omp.h>
#include "/opt/homebrew/opt/libomp/include/omp.h" //had to declare the path in mac

#include "helper.h"
#include "geometry.h"

Helper _help;

using namespace std;

// QString _input_folder_name = "test_sa/data/";
QString _input_folder_name = "../../../test_sa/data/";
QString _input_file_name = "f1";
//  commented the next two lines because there is no last snapshot
QString _input_file_name_last = "output_ox_unspacing_last";
bool do_last = false;

// this is the spacing in the intermediate snapshot output from qdyn
unsigned int _nx_spacing = 2;
unsigned int _nz_spacing = 2;
// downsize the sampling in case it is required
unsigned int _nx = 128 / _nx_spacing;
unsigned int _nz = 128 / _nz_spacing;

unsigned int _ntimes = 692;
bool _rel_t = false;
double _tyrs = 3600 * 24 * 365;

QList<Point> _coords;

unsigned int _running_timestep;
QList<double> _times;
QList<int> _tsteps;
struct VAR
{
  QList<double> values;
};

struct VARS
{
  QList<QString> name;
  QList<VAR> values;
};

VARS _vars;

struct EXODUS_II
{
  int num_dim, num_nodes;
  int num_elem, num_elem_blk, num_nodes_per_elem;
  QList<Node> nodes;
  QList<int> conn;
  int id;
  QString file_name;
  int num_vars;
};

EXODUS_II exo;

void setVar(int post, int posv, double *var, QString filename, bool is_final)
{
  if (is_final)
  {
    // open the file containing the last entry
    QFile file(filename);
    if (!file.exists())
      _help.printError("could not open file " + filename + " to read info, aborting.");
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    QString line;
    int count = 0;
    // this is the starting position of the vector
    int spos = (post - 1) * exo.num_nodes;
    while (!stream.atEnd())
    {
      line = stream.readLine().simplified();
      if (count++ == (spos + 1)) // skip the header
      {
        // retrieve original spacing and dwonsample the file
        int onz = _nz * _nz_spacing;
        int onx = _nx * _nx_spacing;
        unsigned int pos = 0;
        for (int ix = 0; ix < onx; ix++)
          for (int iz = 0; iz < onz; iz++)
          {
            if (ix % _nx_spacing == 0 && iz % _nz_spacing == 0)
            {
              QStringList list;
              list = line.split(',');
              QList<double> sen;
              for (int j = 0; j < list.size(); j++)
                sen.append(list.at(j).toDouble());
              var[pos++] = sen[posv];
            }
            line = stream.readLine().simplified();
          }
        break;
      }
    }
    file.close();
  }
  else
  {
    // this is the starting position in the vector
    int spos = (post - 1) * exo.num_nodes;
    for (int i = 0; i < exo.num_nodes; i++)
      var[i] = _vars.values[spos + i].values[posv];
  }
}

void appendExodus(double time, bool is_final)
{
  // set exodus binary formatting
  int CPU_word_size, IO_word_size, exoid, error;
  float version;
  CPU_word_size = IO_word_size = sizeof(double);
  double *var;
  // open exodus file
  exoid = ex_open(exo.file_name.toLatin1().data(), EX_WRITE, &CPU_word_size, &IO_word_size, &version);
  if (exoid < 0)
    _help.printError("could not open file[" + exo.file_name +
                     "] to append timestep information ... aborting.");
  // append time
  error = ex_put_time(exoid, _running_timestep + 1, &time);
  // append field variable
  for (int i = 0; i < exo.num_vars; i++)
  {
    var = (double *)calloc(exo.num_nodes, CPU_word_size);
    if (is_final)
      setVar(1, i, var, _input_folder_name + _input_file_name_last + ".csv", is_final);
    else
      setVar(_running_timestep + 1, i, var, _input_folder_name + _input_file_name + ".csv", is_final);
    error = ex_put_var(exoid, _running_timestep + 1, EX_NODE_BLOCK, i + 1, exo.id, exo.num_nodes, var);
    free(var);
  }
  // update the instance
  error = ex_update(exoid);
  //  close instance
  error = ex_close(exoid);
}

void createExodus()
{
  int CPU_word_size, IO_word_size, exoid;
  CPU_word_size = sizeof(double);
  IO_word_size = 8;
  int error;
  char *coord_names[3];
  double *x, *y, *z;
  int *node_map, *ele_map, *connect;
  double *var;
  int *truth_tab;
  exo.id = 0;
  // create the exodusii instance
  exo.file_name = _input_folder_name + _input_file_name + ".e";
  exoid = ex_create(exo.file_name.toLatin1().data(), EX_CLOBBER, &CPU_word_size, &IO_word_size);
  // initialize exodusii the instance
  exo.num_nodes = _nx * _nz;
  if (exo.num_nodes != _coords.size())
    _help.printError("exo.num_nodes = " + QString::number(exo.num_nodes) + " - _coords.size = " + QString::number(_coords.size()));
  exo.num_elem = (_nx - 1) * (_nz - 1);
  exo.num_elem_blk = 1;
  exo.num_dim = 3;
  QString name = "fault"; // this is hard coded (in reality for more than one fault he should get the fault_id)
  error = ex_put_init(exoid, name.toLatin1().data(), exo.num_dim, exo.num_nodes, exo.num_elem, exo.num_elem_blk, 0, 0);
  // append coordinates
  coord_names[0] = "x";
  coord_names[1] = "y";
  coord_names[2] = "z";
  error = ex_put_coord_names(exoid, coord_names);
  x = new double[exo.num_nodes];
  y = new double[exo.num_nodes];
  z = new double[exo.num_nodes];
  for (int i = 0; i < exo.num_nodes; i++)
  {
    x[i] = _coords[i].x();
    y[i] = _coords[i].y();
    z[i] = _coords[i].z();
  }
  error = ex_put_coord(exoid, x, y, z);
  // append node num map
  node_map = (int *)calloc(exo.num_nodes, sizeof(int));
  for (int i = 0; i < exo.num_nodes; i++)
    node_map[i] = i + 1;
  error = ex_put_id_map(exoid, EX_NODE_MAP, node_map);
  free(node_map);
  // append element map
  ele_map = (int *)calloc(exo.num_elem, sizeof(int));
  for (int i = 0; i < exo.num_elem; i++)
    ele_map[i] = i + 1;
  error = ex_put_id_map(exoid, EX_ELEM_MAP, ele_map);
  free(ele_map);
  // append element block
  QString ele_type = "QUAD";
  exo.num_nodes_per_elem = 4;
  error = ex_put_block(exoid, EX_ELEM_BLOCK, exo.id, ele_type.toLatin1().data(), exo.num_elem, exo.num_nodes_per_elem, 0, 0, 0);
  //  append element connectivities
  connect = (int *)calloc(exo.num_elem * exo.num_nodes_per_elem, sizeof(int));
  for (int i = 0; i < _nx - 1; i++)
    for (int j = 0; j < _nz - 1; j++)
    {
      int n1 = i + j * _nx;
      int n2 = (i + 1) + j * _nx;
      int n3 = (i + 1) + (j + 1) * _nx;
      int n4 = i + (j + 1) * _nx;
      exo.conn.append(n1 + 1);
      exo.conn.append(n2 + 1);
      exo.conn.append(n3 + 1);
      exo.conn.append(n4 + 1);
    }
  for (int i = 0; i < exo.conn.size(); i++)
    connect[i] = exo.conn[i];
  error = ex_put_conn(exoid, EX_ELEM_BLOCK, exo.id, connect, NULL, NULL);
  free(connect);
  // append field variables name
  exo.num_vars = _vars.name.size();
  error = ex_put_variable_param(exoid, EX_NODE_BLOCK, exo.num_vars);
  for (int i = 0; i < exo.num_vars; i++)
    error = ex_put_variable_name(exoid, EX_NODE_BLOCK, i + 1, _vars.name[i].toLatin1().data());
  //  append truth table for field variables
  truth_tab = (int *)calloc(exo.num_vars, sizeof(int));
  for (int i = 0; i < exo.num_vars; i++)
    truth_tab[i] = 1;
  error = ex_put_truth_table(exoid, EX_NODE_BLOCK, exo.id, exo.num_vars, truth_tab);
  free(truth_tab);
  // append time
  double time_value = _times.first();
  // this is the first hit (it should start at 1)
  _running_timestep = 0;
  error = ex_put_time(exoid, _running_timestep + 1, &time_value);
  // append field variables
  for (int i = 0; i < exo.num_vars; i++)
  {
    var = (double *)calloc(exo.num_nodes, CPU_word_size);
    setVar(_running_timestep + 1, i, var, _input_folder_name + _input_file_name + ".csv", false);
    error = ex_put_var(exoid, _running_timestep + 1, EX_NODE_BLOCK, i + 1, exo.id, exo.num_nodes, var);
    free(var);
  }
  // update the instance
  error = ex_update(exoid);
  //  close instance
  error = ex_close(exoid);
}

void readInput(QString filename)
{
  filename = QDir::toNativeSeparators(filename);
  QFile file(filename);
  if (!file.exists())
    _help.printError("could not open file " + filename + " to read info, aborting.");
  file.open(QIODevice::ReadOnly);
  QTextStream stream(&file);
  QString line;
  unsigned int count = 0;
  unsigned int local_count = 0;
  QStringList sl;
  unsigned int post;
  int posts = -1;
  unsigned int posx, posy, posz;
  while (!stream.atEnd())
  {
    count++;
    line = stream.readLine().simplified();
    if (count < 2) // read the header and fill the var names vector
    {
      sl = line.split(',');
      for (int i = 0; i < sl.size(); i++)
      {
        _vars.name.append(sl.at(i));
        if (sl.at(i) == "t")
          post = i;
        if (sl.at(i) == "step")
          posts = i;
        if (sl.at(i) == "x")
          posx = i;
        if (sl.at(i) == "y")
          posy = i;
        if (sl.at(i) == "z")
          posz = i;
      }
    }
    else // fill in the var values
    {
      // read in coordinates - only once per time step
      if (count < (_nx * _nz) + 2)
      {
        Point pt(line.section(",", posx, posx).toDouble(), line.section(",", posy, posy).toDouble(), line.section(",", posz, posz).toDouble());
        _coords.append(pt);
      }
      // fill in times and timesteps (sanitized)
      if (_times.isEmpty())
      {
        _times.append(line.section(",", post, post).toDouble() / _tyrs);
        _tsteps.append(_times.size() - 1);
      }
      else if (line.section(",", post, post).toDouble() / _tyrs != _times.last())
      {
        _times.append(line.section(",", post, post).toDouble() / _tyrs);
        _tsteps.append(_times.size() - 1);
      }
      // fill in variables (all but the last)
      sl = line.split(',');
      VAR vv;
      for (int i = 0; i < sl.size(); i++)
        vv.values.append(sl.at(i).toDouble());
      _vars.values.append(vv);
      local_count++;
      double percentage = (100.0 * local_count) / (_nx * _nz * _ntimes);
      _help.printStatus("reading in " + QString::number(percentage, 'g', 3) + "% of the total data ...");
    }
  }
  file.close();
  if (_rel_t)
  {
    double tn = _times[0];
    for (int i = 0; i < _times.size(); i++)
      _times[i] /= tn;
  }
}

int main(int argc, char *argv[])
{
  QString usage = "\nUsage: qdyn2ex\n\n";
  QString str;
  QCoreApplication app(argc, argv);
  _help.printInfo("\nstart reading in qdyn output files ...");
  readInput(_input_folder_name + _input_file_name + ".csv");
  _help.printInfo("\ncreating the first instance of the exodus class ...");
  createExodus();
  _help.printInfo("\nfilling in the exodus instance at time steps ... ");
  for (int i = 0; i < _tsteps.size(); i++)
  {
    _help.printStatus("doing time step " + QString::number(_running_timestep + 1) + " out of " + QString::number(_tsteps.size()));
    appendExodus(_times[i], false);
    _running_timestep++;
  }
  // commentted out this statement because there is no last snapshot
  // if (do_last)
  // {
  //   _help.printInfo("\nadd last time step ...");
  //   appendExodus(_times.last(), true);
  // }
  _help.printInfo("\nexport completed!");
  QTimer::singleShot(0, []()
                     { QCoreApplication::exit(0); });
  return app.exec();
}

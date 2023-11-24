#ifndef _HELPER_H
#define _HELPER_H

#include <QDebug>
#include <QTextStream>

struct Helper
{
  Helper(){};
  void print(QString str)
  {
    QTextStream out(stdout);
    out << str << endl;
  }
  void status(QString str)
  {
    QTextStream out(stdout);
    out << "\r" << str << flush;
  }
  void printStatus(QString str) { status("\033[32m" + str + "\033[0m"); }
  void printInfo(QString str) { print("\033[90m" + str + "\033[0m"); }
  void printWarning(QString str) { print("\033[93m" + str + "\033[0m"); }
  void printError(QString str)
  {
    print("\033[91m" + str + "\033[0m");
    exit(EXIT_FAILURE);
  }
};

#endif

#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "QtCore"

#include <iostream>
#include <omp.h>

using namespace std;

struct Point
{
  Point() { xp = yp = zp = 0.0; }
  Point(double xpos, double ypos, double zpos)
  {
    xp = xpos;
    yp = ypos;
    zp = zpos;
  }
  Point(const Point &pt)
  {
    xp = pt.xp;
    yp = pt.yp;
    zp = pt.zp;
  }
  double x() { return xp; }
  double y() { return yp; }
  double z() { return zp; }

  void setX(double x) { xp = x; }
  void setY(double y) { yp = y; }
  void setZ(double z) { zp = z; }

  Point &operator=(Point pt)
  {
    xp = pt.x();
    yp = pt.y();
    zp = pt.z();
    return *this;
  }

  double L2_norm(Point pt)
  {
    return std::sqrt((xp - pt.x()) * (xp - pt.x()) +
                     (yp - pt.y()) * (yp - pt.y()) +
                     (zp - pt.z()) * (zp - pt.z()));
  }

  double L2_norm_sqr(Point pt)
  {
    return L2_norm(pt) * L2_norm(pt);
  }

  Point dist(Point pt)
  {
    return Point(std::sqrt((xp - pt.x()) * (xp - pt.x())),
                 std::sqrt((yp - pt.y()) * (yp - pt.y())),
                 std::sqrt((zp - pt.z()) * (zp - pt.z())));
  }

  double xp, yp, zp;
};

struct Node
{
  Node()
  {
    nodeId = 0;
    pt.xp = pt.yp = pt.zp = 0.0;
  }
  Node(int id, Point npt)
  {
    nodeId = id;
    pt = npt;
  }

  int id() { return nodeId; }
  Point point() { return pt; }
  double x() { return pt.x(); }
  double y() { return pt.y(); }
  double z() { return pt.z(); }

  void setId(int id) { nodeId = id; }
  void setX(double x) { pt.xp = x; }
  void setY(double y) { pt.yp = y; }
  void setZ(double z) { pt.zp = z; }

  bool operator<(Node other)
  {
    if (pt.y() == other.pt.y())
      return (pt.x() < other.pt.x());
    else
      return (pt.y() < other.pt.y());
  }

  int nodeId;
  Point pt;
};

struct Element
{
  int eleId() { return ele_id; }
  void setEleId(int id) { ele_id = id; }
  Node atNode(int pos) { return nodeList.at(pos); }
  Point getCentroid() { return centroid; }

  QList<Node> nodeList;
  Point centroid;
  int ele_id;
};

struct BoundarySet
{
  int boundaryId;
  QString type;
  int location;
};

#endif

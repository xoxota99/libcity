/**
 * This code is part of libcity library.
 *
 * @file area/area.cpp
 * @date 26.04.2011
 * @author Radek Pazdera (xpazde00@stud.fit.vutbr.cz)
 *
 * @see area.h
 *
 */

#include "area.h"

#include "../debug.h"
#include "../geometry/units.h"
#include "../geometry/point.h"
#include "../geometry/polygon.h"
#include "../geometry/vector.h"
#include "../geometry/linesegment.h"

Area::Area()
{
  initialize();
}

Area::Area(Area const& source)
{
  initialize();

  //*constraints = *(source.constraints);
  polygonGraph = copyPolygonGraph(source.polygonGraph);
  parentArea = source.parentArea;
}

void Area::initialize()
{
  parentArea = 0;
  polygonGraph = 0;
  //constraints = new Polygon();
}

Area& Area::operator=(Area const& source)
{
  //*constraints = *(source.constraints);
  polygonGraph = copyPolygonGraph(source.polygonGraph);
  parentArea = source.parentArea;

  return *this;
}

Area::~Area()
{
  freeMemory();
}

void Area::freeMemory()
{
  //delete constraints;
  Edge* first = polygonGraph;
  while(polygonGraph->next != first)
  {
    polygonGraph = polygonGraph->next;
    delete polygonGraph->previous;
  }
}

Polygon Area::areaConstraints()
{
  return constructPolygon(polygonGraph);
  //return constraints;
}

void Area::setAreaConstraints(Polygon const& area)
{
  //*constraints = area;
  assert(area.numberOfVertices() >= 3);
  polygonGraph = constructPolygonGraph(area);
}

void  Area::setParent(Area* area)
{
  parentArea = area;
}

Area* Area::parent()
{
  return parentArea;
}

Area::Edge* Area::getLongestEdgeWithRoadAccess()
{
  Edge* current = polygonGraph;
  Edge* longest = 0;

  while(current->next != polygonGraph)
  {
    if (((current->begining - current->next->begining).length() >
         (longest->begining - longest->next->begining).length() && !current->hasRoadAccess) ||
        (longest == 0 && !current->hasRoadAccess))
    {
      longest = current;
    }
    current = current->next;
  }

  return longest;
}

Area::Edge* Area::getLongestEdgeWithoutRoadAccess()
{
  Edge* current = polygonGraph;
  Edge* longest = 0;

  while(current->next != polygonGraph)
  {
    if (((current->begining - current->next->begining).length() >
         (longest->begining - longest->next->begining).length() && !current->hasRoadAccess) ||
        (longest == 0 && !current->hasRoadAccess))
    {
      longest = current;
    }
    current = current->next;
  }

  return longest;
}

bool Area::hasRoadAccess()
{
  Edge* current = polygonGraph;
  while(current->next != polygonGraph)
  {
    if (current->hasRoadAccess)
    {
      return true;
    }
    current = current->next;
  }

  return false;
}

Area::Edge* Area::constructPolygonGraph(Polygon const& polygon)
{
  assert(polygon.numberOfVertices() >= 3);

  Edge* current = new Edge;
  Edge* first = current;
  Edge* previous = 0;
  for (unsigned int i = 0; i < polygon.numberOfVertices(); i++)
  {
    current->begining = polygon.vertex(i);
    current->hasRoadAccess = false;
    current->next = new Edge;
    current->previous = previous;
    previous = current;
    current = current->next;
  }

  /* Connect the end to the begining */
  current = current->previous;
  delete current->next;
  current->next = first;
  first->previous = current;

  return current;
}

Area::Edge* Area::copyPolygonGraph(Edge* source)
{
  Edge* sourceCurrent = source;

  Edge* current = new Edge;
  Edge* first = current;
  Edge* previous = 0;
  while(sourceCurrent->next != source)
  {
    current->begining = sourceCurrent->begining;
    current->hasRoadAccess = sourceCurrent->hasRoadAccess; // All block edges have road access
    current->next = new Edge;
    current->previous = previous;
    previous = current;
    current = current->next;

    sourceCurrent = sourceCurrent->next;
  }

  /* Connect the end to the begining */
  current = current->previous;
  delete current->next;
  current->next = first;
  first->previous = current;

  return first;
}

Polygon Area::constructPolygon(Edge* graph)
{
  Edge* current = polygonGraph;
  Polygon polygon;
  while(current->next != polygonGraph)
  {
    polygon.addVertex(current->begining);
    current = current->next;
  }

  return polygon;
}

Area::Edge* Area::getPolygonGraph()
{
  return polygonGraph;
}

Area::Edge* Area::getPolygonGraphCopy()
{
  return copyPolygonGraph(polygonGraph);
}

Area::Edge* Area::insertToPolygonGraph(Edge* after, Point const& begining)
{
  assert(after != 0);

  Edge* next = after->next;
  after->next = new Edge;
  after->next->previous = after;
  after->next->next = next;
  next->previous = after->next;

  return after->next;
}

void Area::bridge(Edge* first, Edge* second)
{
  Edge* otherFirst;
  Edge* otherSecond;

  otherFirst = insertToPolygonGraph(first, first->begining);
  otherFirst->hasRoadAccess = false;

  otherSecond = insertToPolygonGraph(second->previous, second->begining);
  otherSecond->hasRoadAccess = false;

  first->next  = otherSecond;
  otherSecond->previous = first;

  second->next = otherFirst;
  otherFirst->previous = second;
}
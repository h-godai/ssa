
//  ŽOŠpŒ`•ªŠ„

#ifndef DIVIDE_POLYGON_HPP
#define DIVIDE_POLYGON_HPP

#ifndef STDAFX_H
#include <string>
#include <iostream>
#include <math.h>
#endif

#include "Vector.h"


namespace ts { namespace divide_polygon {

  typedef struct _Point 
  {
      double  x;
      double  y;
      _Point  *pPrev;
      _Point  *pNext;
  } Point;

  struct DividePolygon
  {
      Point*  orgPoints;
      vector<Vector2Dd>   divided;

      Point     *head; 
      Point     *curr;
      int32_t   counter;
      int32_t   pt_size;

      DividePolygon( vector<Vector2Dd>& points )
      {
          head    = NULL; 
          curr    = NULL;
          counter = 0;
          pt_size = points.size();

          divided.clear();

          orgPoints = new  Point [ pt_size ];
          head      = &orgPoints[0];

          for ( vector<Vector2Dd>::reverse_iterator i = points.rbegin(); i != points.rend(); ++i ) 
          {
              add_Point( *i ); 
          }

          divide( );

          delete [] orgPoints;
      }

      void add_Point( Vector2Dd pt ) 
      {
          Point* pCur = &orgPoints[counter];
          pCur->x = pt.x;
          pCur->y = pt.y;

          int32_t pre =  counter-1;        
          int32_t nxt =  counter+1;        
          if( pre < 0 ) pre = pt_size - 1;
          if( nxt >= pt_size ) nxt = 0;

          pCur->pPrev = &orgPoints[pre];
          pCur->pNext = &orgPoints[nxt];

          counter++;
      }

      void del( Point *curr ) 
      {
          if( curr == NULL )  return;
          counter--;
          if( curr->pPrev == curr || curr->pNext == curr ) 
          {
              head = NULL;
              return;
          }
          if( curr == head )  head = curr->pNext;
          curr->pNext->pPrev = curr->pPrev;
          curr->pPrev->pNext = curr->pNext;
      }

      int wayOfNormal( Point *input1, Point *input2, Point *input3 )  const
      {
          double temp10 = input1->x - input2->x;
          double temp11 = input1->y - input2->y;
          double temp20 = input1->x - input3->x;
          double temp21 = input1->y - input3->y;
          double ret= temp10 * temp21 - temp11 * temp20;
          if ( ret > 0 )  return 1;
          else            return 0;
      }

      int CheckPointInTriangle( Point *p1, Point *p2, Point *p3, Point *check) const
      {
          int result1 = wayOfNormal( check, p1, p2 );
          int result2 = wayOfNormal( check, p2, p3 );
          int result3 = wayOfNormal( check, p3, p1 );
          if( result1 == result2 && 
              result2 == result3 )  return 1;
          else                      return 0;
      }

      int checkIsOtherPointsInside( Point *point ) const
      {
          Point *p = point->pNext->pNext;
          while( 1 ) 
          {
              if( p == NULL ) break;
              if ( CheckPointInTriangle( point, point->pNext, point->pPrev, p ) )  return 1;
              p = p->pNext;
              if ( p == point->pPrev )  break;
          }
          return 0;
      }
            
      Point*  getMaxDistanceFromOrigin( void ) const
      {
          double distance = 0;
          Point *p    = head;
          Point *max  = head;
          while( 1 ) 
          {
              if( p == NULL ) break;
              double abs = sqrt( p->x * p->x + p->y * p->y );
              if( abs > distance ) 
              {
                  distance = abs;
                  max = p;
              }
              p = p->pNext;
              if ( p == head )  break;
          }
          return max;
      }

      void divide( ) 
      {

          while( counter > 2 ) 
          {

              Point *start = getMaxDistanceFromOrigin ( );
              int way = wayOfNormal ( start, start->pPrev, start->pNext );
              Point *p = start;

              while( 1 ) 
              {
                  if( p == NULL ) break;
                  if( way == wayOfNormal ( p, p->pPrev, p->pNext ) )
                  {
                      if( !checkIsOtherPointsInside( p ) ) 
                      {
                          divided.push_back( Vector2Dd( p->x, p->y ) );
                          divided.push_back( Vector2Dd( p->pNext->x, p->pNext->y )  );
                          divided.push_back( Vector2Dd( p->pPrev->x, p->pPrev->y )  );
                          del ( p );
                          break;
                      }
                  }

                  p = p->pPrev;
                  if ( p == start ) break;
              }

          }

      }

  };



} }

#endif


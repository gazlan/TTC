#include "stdafx.h"

#include "buffer_circular.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@ CircularBuffer::CircularBuffer()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

CircularBuffer::CircularBuffer()
{
   _pData = NULL; // Prevent DebugMode Exception

   Reset();
}

/* ******************************************************************** **
** @@ CircularBuffer::~CircularBuffer()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Destructor
** ******************************************************************** */

CircularBuffer::~CircularBuffer()
{
   Reset();
}

/* ******************************************************************** **
** @@ CircularBuffer::Push()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CircularBuffer::Push(DWORD dwItem)
{
   if (_bFull)
   {
      _dwHead  = _dwTail + 1;
      _dwHead %= _dwSize;
   }

   _pData[_dwTail++]  = dwItem;
   _dwTail           %= _dwSize;
   
   if (!_bFull)
   {
      ++_dwCnt;

      if (_dwCnt == _dwSize)
      {
         _bFull = true;
      }
   }
}

/* ******************************************************************** **
** @@ CircularBuffer::Skip()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CircularBuffer::Skip(DWORD dwCnt)
{
   for (DWORD ii = 0; ii < dwCnt; ++ii)
   {
      if (_bFull)
      {
         _dwHead  = _dwTail + 1;
         _dwHead %= _dwSize;
      }

      _pData[_dwTail++]  = 0;
      _dwTail           %= _dwSize;

      if (!_bFull)
      {
         ++_dwCnt;

         if (_dwCnt == _dwSize)
         {
            _bFull = true;
         }
      }
   }
}

/* ******************************************************************** **
** @@ CircularBuffer::Peek()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

DWORD CircularBuffer::Peek()
{
   if (!_dwCnt)
   {
      // Empty
      return 0;
   }
   
   if (_bFull)
   {
      _bFull = false;
   }

   --_dwCnt;

   DWORD    dwTemp = _pData[_dwHead];

   ++_dwHead;
   _dwHead %= _dwSize;

   return dwTemp;
}

/* ******************************************************************** **
** @@ CircularBuffer::First()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

DWORD CircularBuffer::First()
{
   return _pData[_dwHead];
}

/* ******************************************************************** **
** @@ CircularBuffer::Last()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

DWORD CircularBuffer::Last()
{
   return _pData[_dwTail];
}

/* ******************************************************************** **
** @@ CircularBuffer::Reset()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CircularBuffer::Reset()
{
   if (_pData)
   {
      delete[] _pData;
      _pData = NULL;
   }

   _dwHead = 0;
   _dwTail = 0;
   _dwSize = 0;
   _dwCnt  = 0;

   _bFull = false;
}

/* ******************************************************************** **
** @@ CircularBuffer::Flush()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CircularBuffer::Flush()
{
   _dwHead = 0;
   _dwTail = 0;
   _dwCnt  = 0;

   _bFull = false;
}

/* ******************************************************************** **
** @@ CircularBuffer::Resize()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CircularBuffer::Resize(DWORD dwSize)
{
   if (_pData)
   {
      delete[] _pData;
      _pData = NULL;
   }

   Reset();;
   
   _pData = new DWORD[dwSize];

   _dwSize = dwSize;
}

/* ******************************************************************** **
** @@ CircularBuffer::GetSize()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

DWORD CircularBuffer::Size() const
{
   return _dwSize;
}

/* ******************************************************************** **
** @@ CircularBuffer::Count()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

DWORD  CircularBuffer::Count() const
{  
   return _dwCnt;
}

/* ******************************************************************** **
** @@ CircularBuffer::Get()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

DWORD CircularBuffer::Get(DWORD dwPos)
{
   if (dwPos > _dwSize)
   {
      return 0;
   }
   
   return _pData[(_dwHead + dwPos) % _dwSize];
}

/* ******************************************************************** **
**                End of File
** ******************************************************************** */

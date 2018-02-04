#ifndef _BUFFER_CIRCULAR_HPP_
#define _BUFFER_CIRCULAR_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

class CircularBuffer
{
   private:

      DWORD*      _pData;
      DWORD       _dwHead;
      DWORD       _dwTail;
      DWORD       _dwSize;
      DWORD       _dwCnt;
      bool        _bFull;
          
   public:

               CircularBuffer();
      virtual ~CircularBuffer();
            
      void     Push(DWORD dwItem);
      DWORD    Peek();
      DWORD    Get(DWORD dwPos);
      DWORD    First();
      DWORD    Last();
      void     Reset();
      void     Flush();
      void     Resize(DWORD dwSize);
      DWORD    Size() const;
      DWORD    Count() const;
      void     Skip(DWORD dwCnt);
};

#endif

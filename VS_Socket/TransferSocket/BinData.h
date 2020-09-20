#pragma once
#include <algorithm>    // std::swap
using namespace std;
// holds some binary data
struct BinData
{
   BinData()
      : buffer_(0), size_(0), capacity_(0)
   {
   }

   BinData(const BinData& rhs)
      : buffer_(0), size_(0), capacity_(0)
   {
      assign( rhs.buffer_, rhs.size_);
   }

   virtual ~BinData()
   {
      size_ = 0;
      capacity_ = 0;
      if (buffer_)
         delete [] buffer_;
      buffer_ = 0;
   }

   BinData& operator = ( const BinData& rhs)
   {
      BinData tmp(rhs);
      Swap(tmp);
      return *this;
   }

   char operator [] (int index) const
   {
      return buffer_[index];
   }

   char& operator[](int index)
   {
      return buffer_[index];
   }

   void clear()
   {
      SetSize(0);
   }

   void Swap(BinData& rhs)
   {
      std::swap( buffer_, rhs.buffer_);
	  std::swap(size_, rhs.size_);
	  std::swap(capacity_, rhs.capacity_);
   }

   void assign(const char* ptr, size_t len)
   {
      SetSize(0);

      Grow( len );

      memcpy( buffer_, ptr, len);
      size_ = len;
      buffer_[size_] = '\0';
   }

   void append(char ch, size_t len = 1)
   {
      Grow( size_ + len );
      memset( buffer_+size_, ch, len);
      size_ += len;
      buffer_[size_] = '\0';
   }

   void SetSize(size_t new_siz, char fillchar = 0 )
   {
      Grow( new_siz );

      if (new_siz > size_)
         memset( buffer_ + size_, fillchar, new_siz - size_);
      else if (new_siz < size_)
         memset( buffer_ + new_siz, fillchar, size_ - new_siz);
      size_ = new_siz;
      buffer_[size_] = '\0';
   }

   void SetCapacity(size_t new_cap)
   {
      if (new_cap > capacity_)
      {
         char* ptr( new char[ new_cap ] );
         if (ptr)
         {
            if ( buffer_ && size_)
               memcpy(ptr, buffer_, size_);
            swap(ptr, buffer_);
            if (ptr)
               delete [] ptr;
            capacity_ = new_cap;
            buffer_[size_] = '\0';
         }
      }
   }

   inline const char* buffer() const { return (char*) buffer_; }
   inline size_t length() const { return size_; }

protected:
   char* buffer_;
   size_t size_;
   size_t capacity_;

   void Grow(size_t new_cap)
   {
      new_cap ++;
      
      size_t cap = capacity_;
      while (cap < new_cap)
      {
         if (cap > 64)
            cap += cap >> 2;
         else {
            if (cap > 8)
               cap += 16;
            else
               cap += 4;
         }
      }
      SetCapacity(cap);
   }
};

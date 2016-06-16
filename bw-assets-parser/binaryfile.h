#ifndef BINARY_FILE_H_INCLUDED
#define BINARY_FILE_H_INCLUDED

#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <stdint.h>
#include <assert.h>

class IBinaryFile: public std::ifstream
{
public:
    IBinaryFile( char const* fn ):
        std::ifstream( fn, std::ios_base::binary )
    { }
    IBinaryFile( std::string const& fn ):
        std::ifstream( fn.c_str(), std::ios_base::binary )
    { }

    template <class T>
    bool read(T& val) {
        char * ptr = reinterpret_cast<char*>(&val);
        return !this->std::ifstream::read( ptr, sizeof(T) ).fail();
    }

    template <class T>
    bool read(T* arr, int n) {
        char * ptr = reinterpret_cast<char*>(arr);
        return !this->std::ifstream::read( ptr, sizeof(T)*n ).fail();
    }

    bool read(std::string& str) {
        int len = 0;
        if( !this->read(len) )
            return false;
        char * buf=new char[len]; 
        if( !buf || !this->read(buf, len) ) {
            delete[] buf;
            return false;
        }
        str = std::string(buf, buf+len);
        delete[] buf;
        return true;
    }

    template <class T, class _>
    bool readSequence(std::vector<T,_>& seq) {
        int len = 0;
        if( !this->read(len) )
            return false;
        T *buf=new T[len];
        if(!buf || !this->read(buf, len) ) {
            delete[] buf;
            return false;
        }
        seq = std::vector<T,_>(buf, buf+len);
        delete[] buf;
        return true;
    }

    template <class K, class V, class _>
    bool readMap( std::map<K,V,_>& m ) {
        int len = 0;
        if( !this->read(len) )
            return false;
        m.clear();
        K key;
        V value;
        for( int i=0; i<len; ++i ) {
            if(!this->read(key))
                return false;
            if(!this->read(value))
                return false;
            m[key] = value;
        }
        return true;
    }
};

class OBinaryFile: public std::ofstream
{
public:
    OBinaryFile( char const* fn ):
        std::ofstream( fn, std::ios_base::binary )
    { }
    OBinaryFile( std::string const& fn ):
        std::ofstream( fn.c_str(), std::ios_base::binary )
    { }

    template <class T>
    bool write(T const& val) {
        return !this->std::ofstream::write( reinterpret_cast<const char*>(&val), sizeof(T) ).fail();
    }

    template <class T>
    bool write(T const* arr, int n) {
        return !this->std::ofstream::write( reinterpret_cast<const char*>(arr), sizeof(T)*n).fail();
    }

    bool write( std::string const& str ) {
        this->write<uint32_t>( str.size() );
        this->write<char>( str.c_str(), str.size() );
        return !this->fail();
    }

    template <class T, class _>
    bool writeSequence(std::vector<T,_> const& seq) {
        this->write<uint32_t>( seq.size() );
        this->write<T>( &seq[0], seq.size() );
        return !this->fail();
    }
};

#endif


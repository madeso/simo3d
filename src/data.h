#ifndef SIMO_DATA_HPP
#define SIMO_DATA_HPP

#include <string>
#include <src/simo.pb.h>
#include <vector>
#include <memory>

simo::rgba Color(float r, float g, float b);
simo::rgba White();
simo::rgba Black();

simo::material DefaultMaterial();

class Data;

simo::vec3 operator+(const simo::vec3& lhs, const simo::vec3& rhs);
simo::vec3 operator*(const simo::vec3& lhs, float rhs);
simo::vec3 operator/(const simo::vec3& lhs, float rhs);

template<typename T, typename TOpenable>
class Array;

template<typename T, typename TOpenable>
class Assignable {
 public:
  typedef Array<T, TOpenable> ArrayType;

  Assignable(ArrayType* t, int i) : array(t), index(i) {
  }

  void set(const T& t);
  const T& get() const;

  void operator=(const T& t) {
    set(t);
  }

  const T* operator->() const {
    return &get();
  }

 private:
  ArrayType* array;
  int index;
};

template<typename T, typename TOpenable>
class ArrayIt : public std::iterator<std::random_access_iterator_tag, T> {
 public:
  typedef Array<T, TOpenable> ArrayType;
  typedef ArrayIt<T, TOpenable> iterator;

  typedef std::iterator<std::random_access_iterator_tag, T> superclass;
  // typedef typename remove_const<T>::type value_type;
  typedef typename superclass::reference reference;
  typedef typename superclass::pointer pointer;
  typedef typename superclass::difference_type difference_type;

  ArrayIt(ArrayType* a, int i) : array(a), index(i) {}

  void operator+=(int steps) {
    index += steps;
  }

  void operator++() {
    ++index;
  }

  void operator++(int) {
    ++index;
  }

  bool IsSame(const iterator& t) const {
    return array == t.array && index == t.index;
  }
  bool operator==(const iterator& t) const {
    return IsSame(t);
  }
  bool operator!=(const iterator& t) const {
    return !IsSame(t);
  }

  Assignable<T, TOpenable> operator*() {
    return Assignable<T, TOpenable>(array, index);
  }

  const T& get() const;

  const T* operator*() const {
    return &get();
  }
 private:
  ArrayType* array;
  int index;
};

template<typename T, typename TOpenable>
class Array {
 public:
  typedef ArrayIt<T, TOpenable> iterator;
  typedef ArrayIt<const T, TOpenable> const_iterator;
  typedef T value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef int size_type;
  typedef ptrdiff_t difference_type;

  Array(TOpenable* openable, google::protobuf::RepeatedPtrField<T>* array) : openable_(openable), array_(array) {}

  const T& at(int index) const {
    return array_->Get(index);
  }

  void set(int index, const T& t) {
    openable_->assertOpen();
    *array_->Mutable(index) = t;
  }

  iterator begin() {
    return iterator(this, 0);
  }

  iterator end() {
    return iterator(this, size());
  }

  int size() const {
    return array_->size();
  }

 private:
  TOpenable* openable_;
  google::protobuf::RepeatedPtrField<T>* array_;
};

template<typename T, typename TOpenable>
const T& ArrayIt<T, TOpenable>::get() const {
  return array->at(index);
}

template<typename T, typename TOpenable>
void Assignable<T, TOpenable>::set(const T& t) {
  array->set(index, t);
}

template<typename T, typename TOpenable>
const T& Assignable<T, TOpenable>::get() const {
  return array->at(index);
}

class Mesh {
 public:
  Mesh(simo::filedata* d, int ind);
  Array<simo::vec3, Mesh> vertices();

  void open();
  void close();
  void assertOpen();

  std::string toString() const;
 private:
  simo::filedata* data;
  int index; // the mesh index in the array

  std::unique_ptr<simo::mesh> old; // if set we are open for modification, if not we are readonly
};

class View;

class Data {
 public:
  Mesh meshes();

  void import(const std::string& path);
 private:
  // meshes
  // data
  friend class Mesh;
  friend class View;
  simo::filedata data;
};

#endif

#include "idgenerator.h"

IdGenerator::IdGenerator() : mId(1) {}

int IdGenerator::generate() {
  int temp = id();
  next();
  return temp;
}

void IdGenerator::next() { ++mId; }

int IdGenerator::id() const { return mId; }

#ifndef SIMO_IDGENERATOR_HPP
#define SIMO_IDGENERATOR_HPP

class IdGenerator {
 public:
  IdGenerator();
  int generate();

 protected:
  void next();
  int id() const;

 private:
  int mId;
};

#endif

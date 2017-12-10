#ifndef problem_h_
#define problem_h_

#include <string>

struct Problem {

  unsigned    rating;
  unsigned    points;
  std::string dataset;
  unsigned    problem_id;
  unsigned    priority;
  std::string answer;
  std::string skeleton; // answer
  
  void log(std::string label, bool answ=false) const;
  void answer2skeleton();
};

#endif

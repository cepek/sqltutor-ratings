#ifndef rating_h_
#define rating_h_

#include "problem.h"

#include <vector>
#include <set>

class Rating {
public:

  Rating();

  void add(const Problem& p);
  void add_output_dataset(std::string dataset);

  void display_input_answers();
  void display_output_answers();

  void analyze();
  void report();

private:

  std::vector<Problem>  input, output;
  std::set<std::string> output_datasets;

  bool input_answers, output_answers;

  std::vector<double> ipoints, ialg1, ialg2;
  double max_points;
  double corr1, corr2;

  double corr(const std::vector<double>& x, const std::vector<double>& y);
  unsigned substr_count(std::string str, std::string substr);
  unsigned alg1(std::string answer);
  unsigned alg2(std::string answer);

  void l1fit();
  double l1a, l1b;
};

#endif

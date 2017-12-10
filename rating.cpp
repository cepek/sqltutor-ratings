#include "rating.h"
#include <iostream>
#include <cmath>
#include <cctype>

Rating::Rating() : input_answers(false), output_answers(false) {}


void Rating::add(const Problem& p)
{
  if (output_datasets.find(p.dataset) == output_datasets.end()) {
    input.push_back(p);
  }
  else {
    output.push_back(p);
  }
}


void Rating::add_output_dataset(std::string dataset)
{
  output_datasets.insert(dataset);
}


void Rating::report()
{
  for (const auto& inp : input ) inp.log("input ", input_answers);

  std::cout << "\n" << corr1 << "  " << corr2 << "\n";
  if  (!output.empty()) std::cout << "\n";

  for (const auto& inp : output) inp.log("output", output_answers);
}

void Rating::display_input_answers()
{
  input_answers = true;
}

void Rating::display_output_answers()
{
  output_answers = true;
}

double Rating::corr(const std::vector<double>& x, const std::vector<double>& y)
{
  unsigned n = x.size();
  double sx {0}, sy {0}, sxy {0}, sx2 {0}, sy2 {0};
  for (unsigned i=0; i<n; i++)
    {
      sx  += x[i];
      sy  += y[i];
      sxy += x[i]*y[i];
      sx2 += x[i]*x[i];
      sy2 += y[i]*y[i];
    }

  double a = n*sxy - sx*sy;
  double b = std::abs(n*sx2 - sx*sx);
  double c = std::abs(n*sy2-sy*sy);

  double tol = 1e-8;
  if (std::abs(a) < tol || b < tol || c < tol) return 0;

  return a/(std::sqrt(b*c));
}

unsigned Rating::substr_count(std::string str, std::string substr)
{
  std::string::size_type count {0}, ind {0}, n = substr.size();
  for (;;)
    {
      ind = str.find(substr, ind);
      if (ind == std::string::npos) break;
      ind += n;
      count++;
    };

  return count;
}
void Rating::analyze()
{
  unsigned m = input .size();
  unsigned n = output.size();
  max_points = 0;
  ipoints.resize(m, 0.0);       // input points
  for (unsigned i=0; i<m; i++)  {
    ipoints[i] = input[i].points;
    if (ipoints[i] > max_points) max_points = ipoints[i];
  }

  l1fit();
  ialg1.resize  (m, 0.0);      // points' estimates computed from input answers
  for (unsigned i=0; i<m; i++) ialg1[i] = alg1(input[i].skeleton);
  corr1 = corr(ialg1, ipoints);

  ialg2.resize(m, 0.0);       // points' estimates computed from L1 fit
  for (unsigned i=0; i<m; i++) ialg2[i] = alg2(input[i].skeleton);
  corr2 = corr(ialg2, ipoints);

  double corr_sum = corr1 + corr2;
  for (unsigned i=0; i<m; i++)
    {
      double r = (ialg1[i]*corr1 + ialg2[i]*corr2)/corr_sum;
      input[i].rating = r + 0.5;
    }

  for (unsigned i=0; i<n; i++) {
    std::string a = output[i].skeleton;
    double r = (alg1(a)*corr1 + alg2(a)*corr2)/corr_sum;
    output[i].rating = r + 0.5;
  }
}

unsigned Rating::alg1(std::string skeleton)
{
  unsigned points = 0;
  points += substr_count(skeleton, " where ");
  points += substr_count(skeleton, " and ");
  points += substr_count(skeleton, " or ");
  points += substr_count(skeleton, " not ");
  points += substr_count(skeleton, " all ");
  points += substr_count(skeleton, " max ");
  points += substr_count(skeleton, " min ");
  points += substr_count(skeleton, " avg ");
  points += substr_count(skeleton, " in ");

  unsigned join
    = substr_count(skeleton, " join ")
    + substr_count(skeleton, " select "); // not the first select
  points += join*join;

  unsigned left  =  substr_count(skeleton, " left ");
  left +=  substr_count(skeleton, " right ");
  left +=  substr_count(skeleton, " cross ");
  left +=  substr_count(skeleton, " natural ");
  left +=  substr_count(skeleton, " cast ");
  points += left*2;

  points += 3*substr_count(skeleton, " union ");
  points += 3*substr_count(skeleton, " intersect ");
  points += 3*substr_count(skeleton, " except ");

  points += 2*substr_count(skeleton, " group ");
  points += 2*substr_count(skeleton, " having ");

  if (points > 10) points--;
  if (points == 0) points = 1;
  if (points > max_points) points = max_points;

  return points;
}

unsigned Rating::alg2(std::string skeleton)
{
  int points = l1a + l1b*skeleton.length();
  if (points < 1) points = 1;
  if (points > max_points) points = max_points;

  return unsigned(points);
}


void Rating::l1fit()
{
  std::vector<double> p(ipoints.size());
  std::vector<double> x(ipoints.size());
  std::vector<double> l(ipoints.size());

  for (unsigned i=0; i<ipoints.size(); i++)
    {
      p[i] = 1;
      x[i] = input[i].skeleton.length();
      l[i] = input[i].points;
    }

  double sum_r1 = 0;
  for (unsigned k=0; k<100; k++)
    {
      double paa {0}, pab {0}, pbb {0}, pal {0}, pbl {0};
      for (unsigned i=0; i<ipoints.size(); i++)
        {
          paa += p[i];
          pab += p[i]*x[i];
          pbb += p[i]*x[i]*x[i];
          pal += p[i]*l[i];
          pbl += p[i]*x[i]*l[i];
        }

      double d =  paa*pbb - pab*pab;
      double a = (pal*pbb - pab*pbl)/d;
      double b = (paa*pbl - pal*pab)/d;

      double sum_r = 0;
      for (unsigned i=0; i<ipoints.size(); i++)
        {
          double r = std::abs(a + b*x[i] - l[i]);
          if (r < 1e-4) r = 1e-4;
          sum_r += r;
          p[i] = 1/r;
        }

      l1a = a;
      l1b = b;
      if (std::abs(sum_r - sum_r1) < 1e-1) break;
      sum_r1 = sum_r;
    }
}

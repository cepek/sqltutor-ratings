#include "problem.h"
#include <iostream>
#include <iomanip>

void Problem::log(std::string label, bool answ) const
{
  std::cout << "-- " << label << std::setw(3) << rating
	    << std::setw(4) << points
	    << "   dataset " << dataset
	    << " " << problem_id
	    << "  priority = "   << priority;
  if (answ) std::cout << "\n" << answer << "\n";
  std::cout << "\n";
}

void Problem::answer2skeleton()
{
  const unsigned N = answer.length();
  char c {};
  skeleton.clear();
  skeleton.reserve(N);
  bool inws = true, isws;
  for (unsigned i=0; i<N; i++)
    {
      c = std::tolower(answer[i]);
      switch (c) {
      case ' ':
      case '(':
      case ')':
      case '<':
      case '>':
      case '+':
      case '-':
      case '=':
      case ';':
      case ',':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
      case '\v':
	c = ' ';
	isws = true;
	break;
      default:
	isws = false;
	break;
      }

      if (inws && isws) continue;
      
      skeleton.push_back(c);  
      inws = isws;
    }
}

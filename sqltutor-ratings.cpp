#include <pqxx/pqxx>

#include <iostream>
#include <iomanip>

#include "rating.h"

int main(int argc, char* argv[])
{
  Rating rating;

  for (int i=1; i<argc; i++)
    {
      std::string s = argv[i];
      if (s=="-i" || s=="--i" || s=="-input_answers" || s=="--input_answers")
	{
	  rating.display_input_answers();
	  continue; 
	}
      
      if (s=="-o" || s=="--o" || s=="-output_answers" || s=="--output_answers")
	{
	  rating.display_output_answers();
	  continue; 
	}
      
      rating.add_output_dataset(s);
    }
  
  {
    using std::string;
    using std::setw;
    
    string constr = "dbname = sqltutor";
    pqxx::connection connection(constr);
    if (!connection.is_open()) {
      std::cerr << "Cannot open connection : " << constr << std::endl;
      return 1;
    }
    
    pqxx::nontransaction nontrans(connection);
    pqxx::result result(nontrans.exec(
       "select points, dataset, problem_id, priority, answer "
       "from   sqltutor.problems "
       "       join sqltutor.datasets using (dataset_id) "
       "       join sqltutor.answers  using (dataset_id, problem_id) "
       "order by dataset asc, problem_id asc, priority asc "
       ));

    for (pqxx::result::const_iterator
	   iter=result.begin(); iter!=result.end(); ++iter)
      {
	Problem p;
	p.rating = 0;
	p.points =     iter[0].as<unsigned>();
	p.dataset =    iter[1].as<string>();
	p.problem_id = iter[2].as<unsigned>();
	p.priority =   iter[3].as<unsigned>();
	p.answer =     iter[4].as<string>();

	p.answer2skeleton();
	
	rating.add(p);
      }
  }

  rating.analyze();
  rating.report();
}

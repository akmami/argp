#ifndef ARGP_CPP
#define ARGP_CPP

#include <cstring>
#include <map>
#include <set>
#include <sstream>

namespace argp {

	class sstream {
	private:
		std::stringstream stream;
	public:
		sstream() {}

		sstream(std::string str) {
			stream << str;
		}

		sstream& operator >> (std::string& str) {
			this->stream >> str;
			return *this;
		};
	};

	enum parser_option { SINGLE_DASH_IS_MULTIFLAG = 0, SINGLE_DASH_IS_SINGLE_FLAG = 1};

	class parser {
	private:
		std::map<std::string, std::string> parameters;
		std::set<std::string> options;

	public:
		bool parse(int argc, char *argv[], int option=SINGLE_DASH_IS_MULTIFLAG) {
			char* token;
			int argc_length;
			
			if (argc < 2)
				return true;
			
			for ( int i=1; i<argc; i++ ) {
				token = argv[i];
				argc_length = strlen(token);
				
				assert( token[0] == '-' && argc_length > 1 );

				if ( i+1 == argc || argv[i+1][0] == '-') {										// OPTION
					if ( token[1] == '-' )
						goto two_dashes;

					if ( option == SINGLE_DASH_IS_MULTIFLAG )
						goto multiflag_parser;

					single_flag_parser:
					assert( argc_length == 2 );
					this->options.emplace( "-" + std::string (1,  token[1] ) );
					goto done;
					
					multiflag_parser:
					for ( int j=1; j<argc_length; j++) {
						this->options.emplace( "-" + std::string(1,  token[j] ) );
					}
					goto done;
					
					two_dashes:
					assert( argc_length > 2 && token[2] != '-' );
					this->options.emplace( std::string( token ) );
					goto done;

					done:
					continue;
				}
				std::cout << argc_length << " " << token << " " << sizeof(token) << " " << sizeof(char) << std::endl;
				assert( ( argc_length == 2 && token[1]  != '-' ) || ( argc_length > 2 && token[1] == '-' && token[2] != '-' ) );

				this->parameters[ token ] = argv[i+1];												// PARAMETER
				i++;	
			}
			return true;
		};

		inline sstream operator()(std::initializer_list<char const* const> list) const {
			for ( std::initializer_list<char const* const>::iterator it = list.begin(); it != list.end(); ++it) {
				if (this->parameters.find(std::string(*it)) != this->parameters.end())
					return sstream(this->parameters.find(std::string(*it))->second);
			}
			return sstream();

		};

		inline bool operator[](char const* element) const {
			return this->options.find(element) != this->options.end();
		};

		void summary() {
			std::set<std::string>::iterator it;
			std::cout << "Options: ";
			for ( it = this->options.begin(); it != this->options.end(); it++ ) {
				std::cout << *it << " ";
			}
			std::cout << std::endl;

			std::map<std::string, std::string>::iterator it2;
			std::cout << "Parameters: ";
			for ( it2 = this->parameters.begin(); it2 != this->parameters.end(); it2++ ) {
				std::cout << it2->first << " " << it2->second << " ";
			}
			std::cout << std::endl;
		}
	};

}

#endif


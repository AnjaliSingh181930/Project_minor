#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include<algorithm>
#include <chrono>
#include <sstream>
#include <cctype>
#include <regex>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/property_maps/constant_property_map.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/iterator/transform_iterator.hpp> 
//#include <boost/algorithm/adaptors.hpp>

using namespace std;
using namespace boost;

// Struct to store the values related to each pin
struct Values {
    int src;
    int x_cord;
    int y_cord;
    int grid_num;
    int i_o;
    float resistance;

};
// Struct to store vertex properties of big graph
struct VertexProperties {
    string name;
    int number;
    int x ;
    int y ;
    map<string, Values> inputs;
    int grid_vertex_name;
};

struct EdgeProperties {
  float weight;
};

// Struct to store vertex and edge properties of big graph
struct VertexProperties1 {
    int id;
    // map<string, float> res;//from vertex_id to string with weight 
};

// Define the big graph type
typedef adjacency_list<vecS, vecS, directedS, VertexProperties, property<edge_weight_t, float> > Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;

// Define the condensed graph type
typedef adjacency_list<vecS, vecS, directedS, VertexProperties1, property<edge_weight_t, float> > Graph1;
typedef graph_traits<Graph1>::vertex_descriptor Vertex1;

auto now = chrono::system_clock::now().time_since_epoch();  //Get time point
auto start = chrono::duration_cast<chrono::milliseconds>(now).count();  //Convert to milliseconds

float stof(const char* text) {
  float result = 0.0f;
  bool is_negative = false;
  int decimal_place = 0;
  int integer_part = 0;

  // Skip leading whitespace
  while (*text && isspace(*text)) {
    text++;
  }

  // Check for sign
  if (*text == '-') {
    is_negative = true;
    text++;
  } else if (*text == '+') {
    text++;  // Skip '+' sign if present (not strictly necessary for conversion)
  }

  // Read integer part
  while (*text && isdigit(*text)) {
    integer_part = integer_part * 10 + (*text - '0');
    text++;
  }

  // Check for decimal point
  if (*text == '.') {
    text++;
    while (*text && isdigit(*text)) {
      result += static_cast<float>((*text - '0')) / pow(10.0f, ++decimal_place);
      text++;
    }
  }

  // Apply sign and return result
  return (is_negative ? -result : result);
}

string generateMatrixRepresentation(const Graph1& g, const map<int, Vertex>& vertice1) {
    string matrixRepresentation;
    size_t num_vertices = boost::num_vertices(g);
    vector<vector<int>> matrix(num_vertices, vector<int>(num_vertices, 0));

    for (size_t i = 0; i < num_vertices; ++i) {
        Vertex1 v = vertex(i, g);
        graph_traits<Graph1>::adjacency_iterator ai, ai_end;
        for (tie(ai, ai_end) = adjacent_vertices(v, g); ai != ai_end; ++ai) {
            Vertex1 u = *ai;
            matrix[i][u] = 1;
        }
    }

    // Construct the matrix representation string
    for (size_t i = 0; i < num_vertices; ++i) {
        for (size_t j = 0; j < num_vertices; ++j) {
            matrixRepresentation += to_string(matrix[i][j]) + " ";
        }
        matrixRepresentation += "\n";
    }
    return matrixRepresentation;
}

int binarySearchInsertionIndex(const vector<int>& sortedArray, int target) {
  int left = 0;
  int right = sortedArray.size() - 1;

  //cout<<"line58 "<<target<<"  " <<sortedArray.size()<<"   "<<endl;
  // Binary search loop
  if(target == 44000)
  {while (left <= right) {
    int mid = left + (right - left) / 2;
    //cout<<"line63 "<<left<<"  "<<mid<<"  "<<right<<endl;
    // Check if target is found
    if (sortedArray[mid] == target) {
      return mid;  // Element found, return its index
    } else if (sortedArray[mid] < target) {
      left = mid + 1;  // Search in the right half
    } else {
      right = mid - 1;  // Search in the left half
    }
  }}

   else
  {while (left <= right) {
    int mid = left + (right - left) / 2;

    // Check if target is found
    if (sortedArray[mid] == target) {
      return mid;  // Element found, return its index
    } else if (sortedArray[mid] < target) {
      left = mid + 1;  // Search in the right half
    } else {
      right = mid - 1;  // Search in the left half
    }
  }}

  // Element not found, return the insertion index
  return left-1;   // Insertion point is after the last element checked (left index)
}

bool is_letter(char ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool isDigit(const string& str) {
  for (char ch : str) {
    if (ch < '0' || ch > '9') {
      return false;
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
    // Open the file
    fstream outputFile(argv[3], ios::out);
    fstream outputFileG1(argv[5], ios::out);

    ifstream infile(argv[1]);
    ifstream cord(argv[2]);

    if (!infile.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    if (!cord.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    // Create a graph
    Graph g;

    Graph1 g1;
    
    // Maps to store vertices
    map<int, Vertex> vertice;//here int is number
    map<int, Vertex1> vertice1;//here int is id

    vector<int> x_cord, y_cord;
    vector<int> grid_node_map;//stores grid_node_name to help check whether its created before or not
    map <int, int> grid_vertex_name_map;//index is node number and the value is the grid node name in int form,
    map <string, float> edge_value; //from:to format and int is resistance value 
    
    // Read info from the file
    string line;

    while(getline(cord, line)){
        // Skip empty lines and lines starting with comments
        if (line.empty() || line[0] == 'c') {
            continue;
        }
       
        if (line[0] == 'x'){
            size_t pos_open = line.find('[');
            //size_t pos_close = line.find(']');
            line = line.substr(pos_open+2); 
            line = regex_replace(line, regex("^ +| +$|( ) +"), "$1");  
            istringstream iss(line);
            string word;

            // Extract words using getline
            while (getline(iss, word, ' ')) {
                
                if(word!="]"){
                    int word_int = stoi(word);
                    //cout<<word_int<<endl;
                    x_cord.push_back(word_int);
                }
            }  
            while(getline(cord, line)) {
                line = regex_replace(line, regex("^ +| +$|( ) +"), "$1");  
                istringstream iss(line);
                string word;

                // Extract words using getline
                while (getline(iss, word, ' ')) {
                    
                    if(word!="]"&word!=" "){
                        int word_int = stoi(word);
                        //cout<<word_int<<endl;
                        x_cord.push_back(word_int);
                    }
                }
                if(word == "]")
                   break;
                 
            }   
        }

        else if (line[0] == 'y'){
            size_t pos_open = line.find('[');
            //size_t pos_close = line.find(']');
            line = line.substr(pos_open+2);    
            line = regex_replace(line, regex("^ +| +$|( ) +"), "$1"); 
            istringstream iss(line);
            string word;

            // Extract words using getline
            while (getline(iss, word, ' ')) {
                // cout<<word<<endl;
                if(word!="]")
                    y_cord.push_back(stoi(word));
            }   

            while(getline(cord, line)) {
                line = regex_replace(line, regex("^ +| +$|( ) +"), "$1"); 
                // cout<<line;
                istringstream iss(line);
                string word;

                // Extract words using getline
                while (getline(iss, word, ' ')) {
                    
                    if(word!="]"&word!=" "){
                        int word_int = stoi(word);
                        //cout<<word_int<<endl;
                        y_cord.push_back(word_int);
                    }
                }
                if(word == "]")
                   break;
                 
            }  
        } 
    }

    // for (int element : x_cord) {
    //     cout << element << " ";
    // }
    // cout<<endl;

    // for (int element : y_cord) {
    //     cout << element << " ";
    // }
    // cout<<endl;
    
    while (getline(infile, line)) {
        //cout<<line<<endl;
        if (line.empty() || is_letter(line[1]) ){
                outputFile<<line<<endl;
            } 
        else if(line[0]== '/') {
            break;
        }
    }    
   
    // for creating NODES
    int i=0; //temporary var
    while (getline(infile, line)) {
        //cout<<line<<endl;
        if (line.empty()|| is_letter(line[1])){
            i++;
            // cout<<i<<endl;
            if(i==5)
            break;
            
        }  
        else{
            size_t pos_eq = line.find(' ');
            int vertex_num = stoi(line.substr(1, pos_eq));
            string vertex_name = line.substr(pos_eq+1);
          
            VertexProperties vp;
            vp.name = vertex_name;
            vp.number = vertex_num ;
            vp.inputs ={};

            // Add vertex to the graph
            Vertex v = add_vertex(vp, g);
            vertice[vertex_num] = v;
        }
        
    }
    
    //add default value to x and y
    for (const auto& pair : vertice) {
        
        const int& vertex_name = pair.first;
        const Vertex& v = pair.second;
        VertexProperties& vp = g[v];
        vp.x = -1000;
        vp.y = -1000;
        vp.grid_vertex_name = -1;
    }

    // for PORTS
    i=0;
    while (getline(infile, line)) {
        //cout<<line<<endl;
        if(line.empty()){
            i++;
        }
        if(i==3)
            break;
    }
           
    // Reading rest of the file
    i=0;
    while (getline(infile, line)) {
        int from; 
        vector<string> words;
        istringstream iss(line);
        string word;

        // Extract words using getline
        while (getline(iss, word, ' ')) {
            // cout<<word<<"Hii"<<endl;
            words.push_back(word);
        }

        //cout<<"Line152"<<endl;

        if(line.empty()){
            continue;
        }
        
        //cout<<"Line159"<<endl;

        if(line == "*END"){
            break;
        }

        else if(words[0] == "*D_NET"){
            size_t pos_eq = words[1].find('*');
            from = stoi(words[1].substr( pos_eq+1));
        }

        else if(words[0] == "*CONN"){

            int i = 0;

            while (getline(infile, line)){

                if(line.empty()){
                    //cout<<"LINE226"<<line<<endl;
                    break;}
                
                vector<string> parts;
                istringstream iss(line);
                string word;

                // Extract words using getline
                while (getline(iss, word, ' ')) {
                    parts.push_back(word);
                }
                // cout<<parts[0]<<" "<<parts[1]<<" "<<endl;
                
                if(parts[0]== "*I"){
                    int x = stoi(parts[4])*2000;
                    int y = stoi(parts[5])*2000;

                    size_t pos_eq = parts[1].find(':');
                    int vertex_num = stoi(parts[1].substr(1, pos_eq));
                    string key = parts[1].substr(pos_eq+1);

                    int ip_op;
                    if( parts[2] == "I")
                        ip_op = 1;
                    else if( parts[2] == "O")
                        ip_op = 0;

                    // const Vertex& source_vertex = vertice[from];
                    const Vertex& target_vertex = vertice[vertex_num];

                    VertexProperties& input_vp = g[target_vertex];
                    string grid_node_name;

                    grid_node_name = to_string(binarySearchInsertionIndex(x_cord,x));
                    grid_node_name.append(to_string(binarySearchInsertionIndex(y_cord,y)));

                    //add_edge(source_vertex, target_vertex,g);
                    
                    int grid_node_name_int = stoi(grid_node_name);
                    //cout<<"Line332 "<< grid_node_name_int<<endl;

                    //put key in dictionary with value from, x, y
                    input_vp.inputs[key] = {from , x , y , grid_node_name_int , ip_op };

                    auto it = find(grid_node_map.begin(), grid_node_map.end(),grid_node_name_int );

                    if(it!= grid_node_map.end()) {
                        continue;
                    }
                    else{
                        VertexProperties1 vp1;
                        vp1.id = grid_node_name_int;

                        // Add vertex to the graph
                        Vertex1 v1 = add_vertex(vp1, g1);
                        vertice1[grid_node_name_int] = v1;

                        grid_vertex_name_map[input_vp.number] = grid_node_name_int;
                        grid_node_map.push_back(grid_node_name_int);
                    }
                }

               else if(parts[0]== "*P"){
                    int x = stoi(parts[4])*2000;
                    int y = stoi(parts[5])*2000;

                    size_t pos_eq = parts[1].find('*');
                    int vertex_num = stoi(parts[1].substr(pos_eq+1));
                    // cout<<parts[1]<<"   "<<vertex_num<<"  line 354 "<<endl;
                    const Vertex& source_vertex = vertice[vertex_num];
                    
                    VertexProperties& input_vp = g[source_vertex];
                    string  grid_node_name;

                    input_vp.x = x;
                    input_vp.y = y;

                    grid_node_name = to_string(binarySearchInsertionIndex(x_cord,x));
                    grid_node_name.append(to_string(binarySearchInsertionIndex(y_cord,y)));
                    int grid_node_name_int= stoi(grid_node_name);
                    //cout<<"Line373 "<< grid_node_name_int<<endl;

                    input_vp.grid_vertex_name = grid_node_name_int;
                    
                    auto it = find(grid_node_map.begin(), grid_node_map.end(),grid_node_name_int );
                    if(it!= grid_node_map.end()){ 
                        continue;
                    }
                    else{
                        VertexProperties1 vp1;
                        vp1.id = grid_node_name_int;

                        // Add vertex to the graph
                        Vertex1 v1 = add_vertex(vp1, g1);
                        vertice1[grid_node_name_int] = v1;

                        grid_vertex_name_map[input_vp.number] = grid_node_name_int;
                        grid_node_map.push_back(grid_node_name_int);
                    }
                }
            }
        }

        else if(words[0] == "*CAP" || isDigit(words[0])){
            //cout<<line<<"InsideCAP"<<endl;
            continue;
        }

        else if(words[0] == "*RES"){
            while (getline(infile, line)){
                if (line == "*END")
                    break;

                vector<string> parts;
                istringstream iss(line);
                string word;

                // Extract words using getline
                while (getline(iss, word, ' ')) {
                    parts.push_back(word);
                }
                float val = stof(parts[3]);

                int rounded_int = static_cast<int>(round(val * 1000));
                val = static_cast<float>(rounded_int) / 1000;
                // cout<<"line522"<<val<<endl;

                size_t pos_eq_1 = parts[1].find(':');
                int vertex_num_1 = stoi(parts[1].substr(1, pos_eq_1));
                string key_1 = parts[1].substr(pos_eq_1 + 1);

                size_t pos_eq_2 = parts[2].find(':');
                int vertex_num_2 = stoi(parts[2].substr(1, pos_eq_2));
                string key_2 = parts[2].substr(pos_eq_2+1);

                const Vertex& source_vertex = vertice[vertex_num_1];
                const Vertex& target_vertex = vertice[vertex_num_2];
               
                if(vertex_num_2 == from && vertex_num_1 != from){
                    VertexProperties& input_vp = g[source_vertex];
                    input_vp.inputs[key_1].resistance = val;
                    // cout<<"line539"<<val;
                    add_edge(target_vertex, source_vertex, val, g);
                }
                    
                if(vertex_num_2 != from && vertex_num_1 == from){
                    VertexProperties& input_vp = g[target_vertex];
                    input_vp.inputs[key_2].resistance = val;
                    add_edge(source_vertex, target_vertex, val ,g);
                }
                }
            }
        }
    

    // Close the file
    infile.close();

    // calculate resistance i.e., edge weight for graph g1
    for (const auto& pair : vertice) {
        const int& vertex_name = pair.first;
        const Vertex& v = pair.second;
        VertexProperties& vp = g[v];

        if(vp.x!= -1000){
            continue;
        }
        else{
            for (const auto& [key, value] : vp.inputs) {
                string name; 

                if (value.i_o == 1)
                    name = to_string(grid_vertex_name_map[value.src])+ ":" + to_string(value.grid_num);
                if (value.i_o == 0)
                    name = to_string(value.grid_num)+ ":" + to_string(grid_vertex_name_map[value.src]);
                //cout<<name<<endl;
                auto it = edge_value.find(name);
                if(it != edge_value.end()){
                    float R1 = edge_value[name];
                    float R2 = value.resistance;
                    edge_value[name] = (R1+R2)/(R1*R2);
                    
                }
                else{
                    edge_value[name] = value.resistance;
                }
                // if(name == "12:0"){
                //     cout<<"Line 590 "<<name<<"   "<<edge_value[name]<< "  "<<value.src<<endl;
                // }
                // if(name == "14:0"){
                //     cout<<"Line 590 "<<name<<"   "<<edge_value[name]<< "  "<<value.src<<endl;
                // }
                // if(name == "21:0"){
                //     cout<<"Line 590 "<<name<<"   "<<edge_value[name]<< "  "<<value.src<<endl;
                // }
                // if(name == "43:0"){
                //     cout<<"Line 590 "<<name<<"   "<<edge_value[name]<< "  "<<value.src<<endl;
                // }

            }
        }
    }

    //Add edges to graph g1
    for ( auto& [key, value] : edge_value) {
        int rounded_int = static_cast<int>(round(value * 1000));
        value = static_cast<float>(rounded_int) / 1000;
        if(key == "44:0")
            cout << "Key: " << key << ", Value: " << value << endl;

        // Find the position of the colon (:)
        size_t pos = key.find(':');
        // Extract the first part (before the colon)
        int from = stoi(key.substr(0, pos));
        // Extract the second part (after the colon)
        int to = stoi(key.substr(pos + 1));

        const Vertex1& src = vertice1[from];
        const Vertex1& target = vertice1[to];
        add_edge(src, target, value, g1);
   }

    // Print vertices and edges of graph g
    for (const auto& pair : vertice) {
        
        const int& vertex_name = pair.first;
        const Vertex& v = pair.second;
        VertexProperties& vp = g[v];
        
        // Print attributes of the node
        outputFile << "Name: " << vp.name << ", Number: " << vp.number << ", X co-ordinate: " << vp.x << ", Y co-ordinate: "<<vp.y<<", Grid vertex name: "<<vp.grid_vertex_name<<", Input Details: ";
    
        for (auto& [key, value] : vp.inputs) {
            outputFile << " Input pin: " << key <<", "<< "Values: ( "<< value.src <<" "<<value.x_cord<<" "<<value.y_cord<<" "<<value.grid_num<<" "<<value.i_o<<" "<<static_cast<float>(static_cast<int>(round(value.resistance * 1000))) / 1000<< " ),";
        }
        outputFile<<endl;
    }    

    // Print vertices and edges of graph g1
    for (const auto& pair : vertice1) {
        const int& vertex_id = pair.first;
        const Vertex1& v = pair.second;
        const VertexProperties1& vp = g1[v];

        // Write vertex information to outputFileG1
        outputFileG1 << "Vertex ID: " << vp.id << endl;

    }

    // Write the graph to Graphviz DOT file
    ofstream dot_file_g(argv[4]);
    write_graphviz(dot_file_g, g, make_label_writer(get(&VertexProperties::name, g)), 
                   make_label_writer(get(edge_weight, g1)));
    dot_file_g.close();

    ofstream dot_file_g1(argv[6]); 
    write_graphviz(dot_file_g1, g1, make_label_writer(get(&VertexProperties1::id, g1)), 
                   make_label_writer(get(edge_weight, g1)));

    dot_file_g1.close();

    // Print adjacency list
    outputFileG1 <<endl<< "Adjacency List:\n";
    for (auto it = vertice1.begin(); it != vertice1.end(); ++it) {
        Vertex1 v = it->second;
        outputFileG1 << it->first << " -> ";
        graph_traits<Graph1>::adjacency_iterator ai, ai_end;
        for (tie(ai, ai_end) = adjacent_vertices(v, g1); ai != ai_end; ++ai) {
            outputFileG1 << g1[*ai].id << " ";
        }
        outputFileG1 << endl;
    }

    string matrixRepresentation = generateMatrixRepresentation(g1, vertice1);
    outputFileG1 << endl << "Matrix Representation:" << endl;
    outputFileG1 << matrixRepresentation;
    
    // Time calculation
    auto now = chrono::system_clock::now().time_since_epoch();  // Get time point
    auto end = chrono::duration_cast<chrono::milliseconds>(now).count();  // Convert to milliseconds
   
    outputFile << "\n"<<"\n"<<"\n"<<(end-start) << endl; 
    return 0;
}

/*
 * pico_json_wrapper.h
 *
 *  Created on: Mar 22, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_JSON_WRAPPER_H_
#define PICO_JSON_WRAPPER_H_


#include <third_party/json_cpp/json/json.h>
using namespace std;
namespace pico{
    Json::Value parseStringToJsonObj(std::string str){
        Json::Value root;
        Json::Reader reader;
        
        bool parsingSuccessful = reader.parse(str, root);
		if (!parsingSuccessful) {
			// report to the user the failure and their locations in the document.
			std::cout << "Failed to parse configuration\n"
            << reader.getFormattedErrorMessages();
			return NULL;
		}
		return root;
    }
    
    void jsonCPPexample() {
        std::string config_doc ("{   \"encoding\" : \"UTF-8\",\"plug-ins\": [ \"python\", \"c++\", \"ruby\"],\"indent\" : { \"length\" : 3, \"use_space\": true }}");
        Json::Value root;   // will contains the root value after parsing.
        Json::Reader reader;
        
        bool parsingSuccessful = reader.parse(config_doc, root);
        if (!parsingSuccessful) {
            // report to the user the failure and their locations in the document.
            std::cout << "Failed to parse configuration\n"
            << reader.getFormattedErrorMessages();
            return;
        }
        
        // Get the value of the member of root named 'encoding', return 'UTF-8' if there is no
        // such member.
        std::string encoding = root.get("encoding", "UTF-8").asString();
        // Get the value of the member of root named 'encoding', return a 'null' value if
        // there is no such member.
        const Json::Value plugins = root["plug-ins"];
        for (int index = 0; index < plugins.size(); ++index) // Iterates over the sequence elements.
            std::cout<<("plugin index is "<<(plugins[index].asString())<<std::endl;
                        
                        std::cout<<((root["indent"].get("length", 3).asInt());
                                    std::cout<<((root["indent"].get("use_space", true).asBool());
                                                
                                                // At application shutdown to make the new configuration document:
                                                // Since Json::Value has implicit constructor for all value types, it is not
                                                // necessary to explicitly construct the Json::Value object:
                                                root["encoding"] = "myEncoding";
                                                root["indent"]["length"] = "12.3";
                                                root["indent"]["use_space"] = "8";
                                                
                                                Json::StyledWriter writer;
                                                // Make a new JSON document for the configuration. Preserve original comments.
                                                std::string outputConfig = writer.write(root);
                                                
                                                // You can also use streams.  This will put the contents of any JSON
                                                // stream at a particular sub-value, if you'd like.
                                                root["subtree"]="I am the crazy subtree";
                                                //	std::cin >> root["subtree"];
                                                
                                                // And you can write to a stream, using the StyledWriter automatically.
                                                std::cout << "root is this now "<<root<<endl;
        
        }
        
        }
#endif /* PICO_JSON_WRAPPER_H_ */

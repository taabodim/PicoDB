/*
 * collection_manager.h
 *
 *  Created on: Mar 13, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef COLLECTION_MANAGER_H_
#define COLLECTION_MANAGER_H_
#include <pico/pico_collection.h>
#include <pico/pico_record.h>
#include <pico/pico_utils.h>

namespace pico{
class collection_manager{
//this class maintains one pico_collection object on the heap per each collection,
//in a map, for each request, the request processor uses the collection that collection manager
//gives her,thus we avoid creating too many same pico_collections,
    list<pico_collection> all_collections;
    list<std::string> name_of_all_collections;
    std::shared_ptr<pico_collection> currencyCollection;
public:
    collection_manager()
    {
        std::string name("currencyCollection");
        currencyCollection =std::make_shared<pico_collection>(name);
    }
    void load_all_names_of_collections()
    {
        string path("/Users/mahmoudtaabodi/Documents/all_collections");
        std::string ext(".dat");
        path.append(ext);
        std::string filename = path;
        
        std::fstream infileLocal;
     
       // cout << " collection_manager  : load_all_names_of_collections : offset is  " << 0//<< std::endl;
        pico_record collection_read_from_file; //the collection is allowed to be 32 characters
        //long in name, so I am going to use pico_record as a wrapper for the char array that is read
        //from file, i load the colelction names in the key_ of the pico_record
        //I can come up with other size of arrays to use in other situations in pico_record
        //in fact its a wrapper
        
        
        
        offsetType endOfFile_Offset = getEndOfFileOffset(infileLocal);//this function is expensive,
        //we should have a class variable to store endOfFilleOffset

       // cout << " get_offset_of_this_record : offset of end of file is " << endOfFile_Offset //<< std::endl;
        infileLocal.open(filename, std::fstream::in | std::fstream::binary);
        		for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
                     pico_record::max_key_size) {
                    
                        infileLocal.seekg(offset);
                        infileLocal.read((char*) collection_read_from_file.key_,
                         pico_record::max_key_size);
                    
                    if(!collection_read_from_file.getKeyAsString().empty())
                    name_of_all_collections.push_back(collection_read_from_file.getKeyAsString());
                    
                    //cout << " load_all_names_of_collections : record_read_from_file.getKeyAsString() " << collection_read_from_file.getKeyAsString()//<< std::endl;
                    
                }
        
        infileLocal.close();
        
        
    }
    void load_all_collections_in_start_up()
    {//this will load all the collections in a map
    
    }
    bool check_if_collection_is_loaded()
    {//checks if a pico_collection is in the map
    
        return false;
    }
    void load_a_collection(std::string colelctionName)
    {//this loads a collection
        
        
    
    }
    
    std::shared_ptr<pico_collection> getTheCollection(std::string collectionName)
    {
        return currencyCollection;
//        return nullptr;
    }
    
private:

};
}


#endif /* COLLECTION_MANAGER_H_ */

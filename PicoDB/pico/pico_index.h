/*
 * pico_index.h
 *
 *  Created on: Mar 22, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_INDEX_H_
#define PICO_INDEX_H_
#include <list>
#include <pico/pico_collection.h>
#include <pico/pico_utils.h>
#include <logger.h>
using namespace std;
namespace pico {
    
    //requirements of std::string
    class pico_record_node {
        public :
        
        offsetType key_of_pico_record; //this key is calculated based on the hash of each pico record
        offsetType pico_record_offset; //this is the place of record in the file
        logger mylogger;
        std::shared_ptr<pico_record_node> left; //left node in the tree
        std::shared_ptr<pico_record_node> right; //right node in the tree
        void printNode()
        {
            if(left==nullptr)
                std::cout<<"left : null";
            else
                std::cout<<"left : "<< left->key_of_pico_record;
            
            std::cout<<" node : "<<key_of_pico_record;
            
            if(right==nullptr)
                std::cout<<"right : null";
            else
                std::cout<<"right : "<<right->key_of_pico_record;
            
        }
    };
    
    
    class pico_binary_index_tree { //this tree saves all the
    public:
        logger mylogger;
        std::shared_ptr<pico_record_node> root;
        
        pico_binary_index_tree() {
            root = std::shared_ptr<pico_record_node>( new pico_record_node);
        }
        ~pico_binary_index_tree() {
            destroy_tree();
            
        }
        void destroy_tree(std::shared_ptr<pico_record_node> leaf) {
            if(leaf!=nullptr)
            {
                
                    if(leaf->left!=nullptr) destroy_tree(leaf->left);
                    if(leaf->right!=nullptr) destroy_tree(leaf->right);
                    leaf=nullptr;
                
            }
        }
        void deleteNode(std::shared_ptr<pico_record_node> node) {
            
        }
        std::shared_ptr<pico_record_node> remove(std::shared_ptr<pico_record_node> nodeToBeDeleted) {
//           std::shared_ptr<pico_record_node> nodeInTreeToBeDeleted= search_in_tree(nodeToBeDeleted);
//            if (nodeInTreeToBeDeleted==nullptr) {
//                std::cerr << "ERROR: Node does not exists\n";
//                return nullptr;
//            }
//            else
//            {
//            if(nodeInTreeToBeDeleted->left!=nullptr)
//            {
//              //  parentNode->left
//                nodeInTreeToBeDeleted->left=nullptr;
//            }
//                if(nodeInTreeToBeDeleted->right!=nullptr)
//                {
//                    
//                    nodeInTreeToBeDeleted->right=nullptr;
//                }
//                nodeInTreeToBeDeleted = nullptr;
//            }
//            
//            std::shared_ptr<pico_record_node> retval;
//            if (key==nodeToBeDeleted->key_of_pico_record) {
//                
//                if (nodeToBeDeleted->left->key_of_pico_record==-1)  {
//                    retval = nodeToBeDeleted->right;
//                    //delete node;
//                    
//                } else if (nodeToBeDeleted->right == NULL) {
//                    retval = node->left;
//                    node=nullptr;
//                    
//                } else {
//                    std::shared_ptr<pico_record_node> successor = getSuccessor(node->left);
//                    node->key_of_pico_record = successor->key_of_pico_record;
//                    node->left = remove(node->left, successor->key_of_pico_record);
//                }
//            } else if (key < node->key_of_pico_record) {
//                node->left = remove(node->left, key);
//            } else {
//                node->right = remove(node->right, key);
//            }
            
            return nullptr;
        }
        
        std::shared_ptr<pico_record_node> getSuccessor(std::shared_ptr<pico_record_node> node) {
            while (node->right->key_of_pico_record!=-1)
                node = node->right;
            return node;
        }
        
        void insert(offsetType offset,std::string key, std::shared_ptr<pico_record_node> leaf) {
            
            std::shared_ptr<pico_record_node>  node = createANodeBasedOnOffsetAndKeyOfRecord(offset,key);
            insert(node, leaf);
        }
        void insert(std::shared_ptr<pico_record_node> nodeToBeInserted, std::shared_ptr<pico_record_node> leaf) {
            offsetType key = nodeToBeInserted->key_of_pico_record;
            offsetType offset= nodeToBeInserted->pico_record_offset;
           // std::cout<<("pico_index : insert : nodeToBeInserted->key_of_pico_record =key = "<<nodeToBeInserted->key_of_pico_record<<endl;
          //  std::cout<<("pico_index : insert : leaf->key_of_pico_record = "<<leaf->key_of_pico_record<<endl;
            
            if (key < leaf->key_of_pico_record) {
                if (leaf->left != nullptr)
                {
                 //   std::cout<<("pico_index : insert : key is smaller checking the left node );
                    
                    insert(nodeToBeInserted, leaf->left);
                }
                else {
                    std::cout<<("pico_index : insert : key is smaller adding a left node to this node");
                    
                    leaf->left = std::shared_ptr<pico_record_node>(new pico_record_node());
                    leaf->left->key_of_pico_record = key;
                    leaf->left->pico_record_offset = offset;
                    leaf->left->left = nullptr; //Sets the left child of the child std::string to nullptr
                    leaf->left->right = nullptr; //Sets the right child of the child std::string to nullptr
                }
            } else if (key >= leaf->key_of_pico_record) {
                if (leaf->right != nullptr)
                {
                    std::cout<<("pico_index : insert : key is larger checking the right node ");
                    
                    insert(nodeToBeInserted, leaf->right);
                }
                else {
                    std::cout<<("pico_index : insert : key is larger adding a right node to this node");
                    
                    leaf->right = std::shared_ptr<pico_record_node>(new pico_record_node());
                    leaf->right->key_of_pico_record = key;
                    leaf->right->pico_record_offset = offset;
                    leaf->right->left = nullptr; //Sets the left child of the child std::string to nullptr
                    leaf->right->right = nullptr; //Sets the right child of the child std::string to nullptr
                }
            }
        }
        std::shared_ptr<pico_record_node> search(offsetType key, std::shared_ptr<pico_record_node> leaf) {
            if (leaf != nullptr) {
                if (key == leaf->key_of_pico_record)
                    return leaf;
                if (key < leaf->key_of_pico_record)
                    return search(key, leaf->left);
                else
                    return search(key, leaf->right);
            } else
                return nullptr;
        }
        
        void insert(std::shared_ptr<pico_record_node> node) {
            insert(node,root);
        }
        std::shared_ptr<pico_record_node> search(offsetType key) {
            return search(key, root);
        }
        void destroy_tree() {
            destroy_tree(root);
        }
        
        void insert(std::string key, std::shared_ptr<pico_record_node> leaf) {
            
        }
        std::shared_ptr<pico_record_node> createANodeBasedOnOffsetAndKeyOfRecord(offsetType offset,std::string key)
        {
            std::shared_ptr<pico_record_node>  node (new pico_record_node());
            std::hash<std::string> hash_fn;
            std::size_t key_hash = hash_fn(key);
            
            node->key_of_pico_record=key_hash;
            node->pico_record_offset =  offset;
            node->left=nullptr;
            node->right=nullptr;
            return node;
        }
        
        void printTheTree()
        {
//            std::cout<<("this is the tree ");
            list<std::shared_ptr<pico_record_node>> level;
            level.push_front(root);
            while(!level.empty()){
                std::shared_ptr<pico_record_node> node = level.front();
                level.pop_front();
                  node->printNode();
                if(node->left!= nullptr)
                {
                   
                    node->left->printNode();
                    level.push_front(node->left);
                }
              //  std::cout<<(node->pico_record_offset);
                if(node->right!= nullptr)
                {
                    node->right->printNode();
                    level.push_front(node->right);
                }
                
            }
            
        }
        void add_to_tree(pico_record& it) { //this method creates a tree structure
            //based on the pico records that it gets reads from a collection
            std::cout<<("adding record to tree ");
            
            std::shared_ptr<pico_record_node>  node = createANodeBasedOnOffsetAndKeyOfRecord(it.offset_of_record,it.getKeyAsString());
            insert(node);
            
        }
        void build_tree(list<pico_record> all_pico_records) { //this method creates a tree structure
            //based on the pico records that it gets reads from a collection
           // std::cout<<("tree is going to be build by "<<all_pico_records.size()<<" elements"<<std::endl;
            
            for (list<pico_record>::iterator it=all_pico_records.begin(); it != all_pico_records.end(); ++it) {
                
             //   std::cout<<("build_tree : offset is "<<it->offset_of_record<<endl;
                std::shared_ptr<pico_record_node>  node = createANodeBasedOnOffsetAndKeyOfRecord(it->offset_of_record,it->getKeyAsString());
                insert(node);
                
            }
            
        }
        
    };
}

#endif /* PICO_INDEX_H_ */

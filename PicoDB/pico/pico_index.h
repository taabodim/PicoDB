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
        
        offsetType key; //this key is calculated based on the hash of the first  pico record of a pico message
        offsetType offset; //this is the offset  of the first record of a pico message  in the file
        //logger mylogger;
        pico_record_node* left; //left node in the tree
        pico_record_node* right; //right node in the tree
        
        offsetType minKey() {
            if (left == NULL)
                return key;
            else
                return left->minKey();
        }
        pico_record_node* deleteNode(offsetType key,pico_record_node *parent) {
            if (key < this->key) {
                if (this->left != NULL)
                    return left->deleteNode(key, this);
                else
                    return NULL;
            } else if (key > this->key) {
                if (this->right != NULL)
                    return right->deleteNode(key, this);
                else
                    return NULL;
            }else {
                if (left != NULL && right != NULL) {
                    this->key = right->minKey();
                    return right->deleteNode(this->key, this);
                } else if (parent->left == this) {
                    
                    parent->left = (left != NULL) ? left : right ;
                    return this;
                } else if (parent->right == this) {
                    
                    parent->right = (left != NULL) ? left : right ;
                    return this;;
                }
            }
            
            
            return NULL;
        }
        
        //        void printNode()
        //        {
        //
        //            std::cout<<"printing a node : \n";
        //            if(left==nullptr)
        //                std::cout<<"left : null ==> ";
        //            else
        //                std::cout<<"left : "<< left->key<<" ==> ";
        //
        //            std::cout<<" node : "<<key;
        //
        //            if(right==nullptr)
        //                std::cout<<" <== right : null";
        //            else
        //                std::cout<<" <== right : "<<right->key;
        //
        //        }
    };
    
    
    class pico_binary_index_tree { //this tree saves all the
    public:
        
        pico_record_node* root;
        long numberOfNodesInTree;
        pico_binary_index_tree() {
            root =  new pico_record_node();
        }
        ~pico_binary_index_tree() {
            destroy_tree();
            
        }
        void destroy_tree(pico_record_node* leaf) {
            if(leaf!=nullptr)
            {
                
                if(leaf->left!=nullptr) destroy_tree(leaf->left);
                if(leaf->right!=nullptr) destroy_tree(leaf->right);
                delete leaf;
                
            }
            
        }
        
        
        bool remove(pico_record_node record) {
            offsetType key =record.key;
            bool retResult;
            if (root == nullptr)
                retResult= false;
            else {
                if (root->key == key) {
                    pico_record_node* auxRoot = new pico_record_node();
                    auxRoot->key=0;
                    auxRoot->left=root;
                    pico_record_node* removedNode = root->deleteNode(key, auxRoot);
                    root = auxRoot->left;
                    if (removedNode != NULL) {
                        delete removedNode;
                        retResult= true;
                    } else
                        retResult=false;
                } else {
                    pico_record_node* removedNode = root->deleteNode(key, NULL);
                    if (removedNode != NULL) {
                        delete removedNode;
                        retResult=true;
                    } else
                        retResult= false;
                }
            }
            
            if(retResult==true)
                numberOfNodesInTree--;
            return retResult;
        }
        //
        //
        //        pico_record_node* getSuccessor(pico_record_node* node) {
        //            while (node->right->key!=-1)
        //                node = node->right;
        //            return node;
        //        }
        
        void insert(pico_record& record, pico_record_node* leaf) {
            
            pico_record_node*  node = convert_pico_record_to_index_node(record);
            insert(node, leaf);
        }
        void insert(pico_record_node* nodeToBeInserted, pico_record_node* leaf) {
            offsetType key = nodeToBeInserted->key;
            offsetType offset= nodeToBeInserted->offset;
            std::cout<<"pico_index : insert : nodeToBeInserted->key =key = "<<nodeToBeInserted->key<<endl;
            std::cout<<"pico_index : insert : leaf->key = "<<leaf->key<<endl;
            
            if (key < leaf->key) {
                if (leaf->left != nullptr)
                {
                    std::cout<<"pico_index : insert : key is smaller checking the left node";
                    
                    insert(nodeToBeInserted, leaf->left);
                }
                else {
                    std::cout<<("pico_index : insert : key is smaller adding a left node to this node");
                    
                    leaf->left = nodeToBeInserted;
                    leaf->left->left = nullptr; //Sets the left child of the child std::string to nullptr
                    leaf->left->right = nullptr; //Sets the right child of the child std::string to nullptr
                }
            } else if (key >= leaf->key) {
                if (leaf->right != nullptr)
                {
                    std::cout<<("pico_index : insert : key is larger checking the right node ");
                    
                    insert(nodeToBeInserted, leaf->right);
                }
                else {
                    std::cout<<("pico_index : insert : leaf doesnt have right child node.");
                    
                    leaf->right = nodeToBeInserted;
                    leaf->right->left = nullptr; //Sets the left child of the child std::string to nullptr
                    leaf->right->right = nullptr; //Sets the right child of the child std::string to nullptr
                }
            }
        }
        pico_record_node* search(pico_record_node* node, pico_record_node* leaf) {
            offsetType key = node->key;
            if (leaf != nullptr) {
                if (key == leaf->key)
                    return leaf;
                if (key < leaf->key)
                    return search(node, leaf->left);
                else
                    return search(node, leaf->right);
            } else
                return nullptr;
        }
        
        void insert(pico_record_node* node) {
            insert(node,root);
        }
        pico_record_node* search(pico_record& record) {
            
            return search(convert_pico_record_to_index_node(record), root);
        }
        void destroy_tree() {
            destroy_tree(root);
        }
        
        pico_record_node* convert_pico_record_to_index_node(pico_record& record)
        {
            pico_record_node*  node (new pico_record_node());
            std::hash<std::string> hash_fn;
            std::size_t key_hash = hash_fn(record.getKeyAsString()); //record key will be used to make the node key
            
            node->key=key_hash;
            // node->key = offset;//for testing the tree, just use the offset for now
            
            node->offset =  record.offset_of_record; //this is the record offset that will be saved in the index
            node->left=nullptr;
            node->right=nullptr;
            return node;
        }
        
        //        void print_tree()
        //        {
        //            std::cout<<"this is the tree \n";
        //            list<pico_record_node*> level;
        //            level.push_front(root);
        //            while(!level.empty()){
        //                pico_record_node* node = level.front();
        //                level.pop_front();
        //
        ////                if(node->left!= nullptr && node->right!= nullptr)
        ////                {//to avoid duplicate printing of a node , only if its really a parent , it will print
        //                    std::cout<<"\nthis is the middle node being printed ";
        //                    node->printNode();
        ////                }
        //                if(node->left!= nullptr)
        //                {
        //                    std::cout<<"\nthis is left Node of the above parent being printed \n";
        //
        //                    node->left->printNode();
        //                    level.push_front(node->left);
        //                }
        //                //  std::cout<<(node->offset);
        //                if(node->right!= nullptr)
        //                {
        //                    std::cout<<"\nthis is right Node of the above parent being printed \n";
        //
        //                    node->right->printNode();
        //                    level.push_front(node->right);
        //                }
        //
        //            }
        //
        //        }
        void print_tree()
        {
            print_tree(root);
        }
        void print_tree(pico_record_node* topNode)//preorderPrint
        {
            if(topNode!=NULL)
            {std::cout<<topNode->key<<" ";
                print_tree(topNode->left);
                print_tree(topNode->right);
            }
        }
        void add_to_tree(pico_record& it) { //this method creates a tree structure
            //based on the pico records that it gets reads from a collection
            std::cout<<("adding record to tree ");
            
            pico_record_node*  node = convert_pico_record_to_index_node(it);
            insert(node);
            numberOfNodesInTree++;
            
        }
        void build_tree(list<pico_record> all_pico_records) { //this method creates a tree structure
            //based on the pico records that it gets reads from a collection
            // std::cout<<("tree is going to be build by "<<all_pico_records.size()<<" elements"<<std::endl;
            
            for (list<pico_record>::iterator it=all_pico_records.begin(); it != all_pico_records.end(); ++it) {
                
                //   std::cout<<("build_tree : offset is "<<it->offset_of_record<<endl;
                pico_record_node*  node = convert_pico_record_to_index_node(*it);
                insert(node);
                numberOfNodesInTree++;
            }
            
        }
        
        void test_tree()
        {
            const int num=20;
            pico_record_node* allNodesInsertedInTree[num];
            pico_record* allRecordsInsertedInTree[num];
            
            string key("mahmoudkey");
            for(int i=0;i<num;i++)
            {
                int offset = random_number<int>(3);
                pico_record record;
                record.offset_of_record = offset;
                record.setKey(random_string(key,5));
                allNodesInsertedInTree[i]=convert_pico_record_to_index_node(record);
                allRecordsInsertedInTree[i]=&record;
                insert(allNodesInsertedInTree[i]);
            }
            
            //  print_tree();
            //remove(allNodesInsertedInTree[1]->key);
            //std::cout<<"this is the tree after deletion of a node \n";
            std::cout<<"this is wehere the second record  is"<<search(*allRecordsInsertedInTree[1]);
            // print_tree();
            
        }
        
    };
}

#endif /* PICO_INDEX_H_ */

#include<iostream>
#include<vector>
#include<memory>
#include<string>

using namespace std;

class File {
    private:
        string name;
        size_t size;
        bool directory;
        vector<shared_ptr<File>> children;
    public:
        File(string name, size_t size, bool isDirectory) : name(name), size(size), directory(isDirectory) {}
        
        void addChild(shared_ptr<File> child) {
            if(directory) {
                children.push_back(child);
            }
        }
        
        string getName() {
            return name;
        }
        size_t getSize() {
            return size;
        }
        bool isDirectory() {
            return directory;
        }
        vector<shared_ptr<File>> getChildren() {
            return children;
        }
};

class Filter {
    public:
        virtual ~Filter() = default;
        virtual bool apply(shared_ptr<File> file) = 0;
};

class NameFilter : public Filter {
    private:
        string name;
    public:
        NameFilter(string name) : name(name) {}
        
        bool apply(shared_ptr<File> file) {
            return file->getName() == name;
        }
};

class ExtensionFilter : public Filter {
    private:
        string extension;
    public:
        ExtensionFilter(string ext) : extension(ext) {}
        
        bool apply(shared_ptr<File> file) {
            if(file->isDirectory()) {
                return false;
            }
            
            string fileName = file->getName();
            if(fileName.length() < extension.length()) {
                return false;
            }
            
            return fileName.substr(fileName.length() - extension.length()) == extension;
        }
};

class SizeGreaterFilter : public Filter {
    private:
        size_t minSize;
    public:
        SizeGreaterFilter(size_t size) : minSize(size) {}
        
        bool apply(shared_ptr<File> file) {
            return file->getSize() > minSize;
        }
};

class TypeFilter : public Filter {
    private:
        bool directory;
    public:
        TypeFilter(bool isDirectory) : directory(isDirectory) {}
        
        bool apply(shared_ptr<File> file) {
            return file->isDirectory() == directory;
        }
};

class AndFilter : public Filter {
    private:
        vector<shared_ptr<Filter>> filters;
    public:
        AndFilter(vector<shared_ptr<Filter>> filters) : filters(filters) {}
        
        bool apply(shared_ptr<File> file) {
            for(auto &f: filters) {
                if(!f->apply(file)) {
                    return false;
                }
            }
            return true;
        }
 };
 
 class OrFilter : public Filter {
     private:
        vector<shared_ptr<Filter>> filters;
    public:
        OrFilter(vector<shared_ptr<Filter>> filters) : filters(filters) {}
        
        bool apply(shared_ptr<File> file) {
            for(auto &f: filters) {
                if(f->apply(file)) {
                    return true;
                }
            }
            return false;
        }
 };
 
 class Finder {
    public:
        vector<shared_ptr<File>> find(
            shared_ptr<File> root, shared_ptr<Filter> filter) {
                vector<shared_ptr<File>> result;
                dfs(root, filter, result);
                return result;
            }
    private:
        void dfs(shared_ptr<File> node, shared_ptr<Filter> filter, vector<shared_ptr<File>> &result) {
            if(filter->apply(node)) {
                result.push_back(node);
            }
            
            if(node->isDirectory()) {
                for(auto &child: node->getChildren()) {
                    dfs(child, filter, result);
                }
            }
        }
 };
 
 int main() {
     
     auto root = make_shared<File>("root", 0, true);
     
     auto dir1 = make_shared<File>("documents", 0, true);
     auto dir2 = make_shared<File>("image", 0, true);
     
     auto file1 = make_shared<File>("resume.txt", 1200, false);
     auto file2 = make_shared<File>("notes.txt", 800, false);
     auto file3 = make_shared<File>("photo.jpg", 5000, false);
     auto file4 = make_shared<File>("bigdata.txt", 10000, false);
     
     root->addChild(dir1);
     root->addChild(dir2);
     
     dir1->addChild(file1);
     dir1->addChild(file2);
     dir1->addChild(file4);
     
     dir2->addChild(file3);
     
     auto txtFilter = make_shared<ExtensionFilter>(".txt");
     auto sizeFilter = make_shared<SizeGreaterFilter>(1000);
     
     vector<shared_ptr<Filter>> filters = {txtFilter, sizeFilter};
     
     auto andFilter = make_shared<AndFilter>(filters);
     
     Finder finder;
     auto results = finder.find(root, andFilter);
     
     cout<<"Matching Files : \n";
     for(auto &file: results) {
         cout<<file->getName()<<" (Size : "<<file->getSize()<< ")\n";
     }
     return 0;
 }

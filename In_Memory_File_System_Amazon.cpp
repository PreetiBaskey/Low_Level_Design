#include <bits/stdc++.h>
using namespace std;

class Node {
public:
    string name;
    bool isFile;

    Node(string name, bool isFile) {
        this->name = name;
        this->isFile = isFile;
    }

    virtual ~Node() {}
};

class File : public Node {
public:
    string content;

    File(string name) : Node(name, true) {}

    void appendContent(string data) {
        content += data;
    }

    string getContent() {
        return content;
    }
};

class Directory : public Node {
public:
    unordered_map<string, Node*> children;

    Directory(string name) : Node(name, false) {}

    bool hasChild(string name) {
        return children.find(name) != children.end();
    }

    Node* getChild(string name) {
        return children[name];
    }

    void addChild(string name, Node* node) {
        children[name] = node;
    }
};

class FileSystem {
private:
    Directory* root;

    vector<string> split(string path) {
        vector<string> tokens;
        stringstream ss(path);
        string token;

        while(getline(ss, token, '/')) {
            if(!token.empty()) tokens.push_back(token);
        }
        return tokens;
    }

    Node* traverse(string path) {
        Directory* curr = root;
        vector<string> parts = split(path);

        for(int i = 0; i < parts.size(); i++) {
            if(!curr->hasChild(parts[i])) return nullptr;

            Node* next = curr->getChild(parts[i]);

            if(i == parts.size() - 1) return next;

            curr = dynamic_cast<Directory*>(next);
        }

        return curr;
    }

public:
    FileSystem() {
        root = new Directory("/");
    }

    vector<string> ls(string path) {
        Node* node = traverse(path);
        vector<string> result;

        if(node == nullptr) return result;

        if(node->isFile) {
            result.push_back(node->name);
            return result;
        }

        Directory* dir = dynamic_cast<Directory*>(node);

        for(auto &it : dir->children) {
            result.push_back(it.first);
        }

        sort(result.begin(), result.end());
        return result;
    }

    void mkdir(string path) {
        Directory* curr = root;
        vector<string> parts = split(path);

        for(string &part : parts) {
            if(!curr->hasChild(part)) {
                curr->addChild(part, new Directory(part));
            }
            curr = dynamic_cast<Directory*>(curr->getChild(part));
        }
    }

    void addContentToFile(string filePath, string content) {
        Directory* curr = root;
        vector<string> parts = split(filePath);

        for(int i = 0; i < parts.size(); i++) {
            string part = parts[i];

            if(i == parts.size() - 1) {
                // file
                if(!curr->hasChild(part)) {
                    curr->addChild(part, new File(part));
                }

                File* file = dynamic_cast<File*>(curr->getChild(part));
                file->appendContent(content);
            } else {
                if(!curr->hasChild(part)) {
                    curr->addChild(part, new Directory(part));
                }
                curr = dynamic_cast<Directory*>(curr->getChild(part));
            }
        }
    }

    string readContentFromFile(string filePath) {
        Node* node = traverse(filePath);
        if(node && node->isFile) {
            File* file = dynamic_cast<File*>(node);
            return file->getContent();
        }
        return "";
    }
};


int main() {
    FileSystem fs;

    fs.mkdir("/a/b/c");
    fs.addContentToFile("/a/b/c/file.txt", "Hello ");
    fs.addContentToFile("/a/b/c/file.txt", "World");

    cout << fs.readContentFromFile("/a/b/c/file.txt") << endl;

    vector<string> files = fs.ls("/a/b/c");
    for(auto &f : files) cout << f << " ";

    return 0;
}

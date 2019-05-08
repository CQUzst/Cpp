#include <iostream>
using namespace std;
enum COLOR{RED,BLACK};

struct RBNode{
    int key;
    COLOR color;
    RBNode* p;
    RBNode* left;
    RBNode* right;
    //参数初始化构造函数
    RBNode(int k, COLOR c = RED, RBNode* parent = nullptr,
           RBNode* l = nullptr, RBNode* r = nullptr) :
        key(k), color(c), p(parent), left(l), right(r) {}
};

class RBTree{
    friend ostream& operator<<(ostream& o, const RBTree& t);
private:
    RBNode* root;
    //左旋
    void leftR(RBNode* n) {
        if (!n || !n->right) {
            return;
        }
        RBNode* r = n->right;
        RBNode* p = n->p;
        if (p) {
            if (p->right == n)
                p->right = r;
            else
                p->left = r;
        }
        else
            root = r;
        r->p = p;
        n->right = r->left;
        if(n->right)
            n->right->p = n;
        r->left = n;
        n->p = r;
    }
    //右旋
    void rightR(RBNode* n) {
        if (!n || !n->left) {
            return;
        }
        RBNode* l = n->left;
        RBNode* p = n->p;
        if (p) {
            if (p->left == n)
                p->left = l;
            else
                p->right = l;
        }
        else
            root = l;
        l->p = p;
        n->left = l->right;
        if(n->left)
            n->left->p = n;
        l->right = n;
        n->p = l;
    }
    //调整
    void keep(RBNode* tokeep) {
        //要调整的节点是红色，循环进行调整
        while (tokeep->p && tokeep->p->color == RED) {
            if (tokeep->p->p->left == tokeep->p) {//其父为左孩子
                RBNode* father = tokeep->p;
                RBNode* uncle = father->p->right;
                //如果父节点和叔节点都是红色，直接调整父和叔颜色为黑，祖父为红，循环p为祖父网上替换颜色
                if (uncle && uncle->color == RED) {
                    father->color = BLACK;
                    uncle->color = BLACK;
                    father->p->color = RED;
                    tokeep = father->p;
                }
                //如果叔节点是黑
                else {
                    //如果tokeep是右子树，先将父节点左旋，更新位置，即转换成tokeep是左子树的情况
                    if (tokeep == father->right) {
                        leftR(father);
                        tokeep = father;
                        father = father->p;
                    }
                    //更新颜色，再右旋
                    father->color = BLACK;
                    father->p->color = RED;
                    rightR(father->p);
                }
            }
            //右边情况和左边对称即可
            else {
                RBNode* father = tokeep->p;
                RBNode* uncle = father->p->left;
                if (uncle && uncle->color == RED) {
                    uncle->color = BLACK;
                    father->color = BLACK;
                    father->p->color = RED;
                    tokeep = father->p;
                }
                else {
                    if (tokeep == father->left) {
                        rightR(father);
                        tokeep = father;
                        father = father->p;
                    }
                    father->color = BLACK;
                    father->p->color = RED;
                    leftR(father->p);
                }
            }
        }
        root->color = BLACK;
    }
    //重载<<前序遍历cout
    ostream& pr(ostream& o,RBNode* r) const{
        if (!r)
            return o;
        o << r->key << " ";
        pr(o, r->left);
        pr(o, r->right);
        return o;
    }

    ostream& mr(ostream& o, RBNode* r) const{
        if (!r)
            return o;
        mr(o, r->left);
        o << r->key << " ";
        mr(o, r->right);
        return o;
    }

    ostream& er(ostream& o, RBNode* r) const{
        if (!r)
            return o;
        er(o, r->left);
        er(o, r->right);
        o << r->key << " ";
        return o;
    }
    //取值
    RBNode* getKey(int key) {
        RBNode* r = root;
        while (r) {
            if (r->key == key)
                break;
            if (r->key < key)
                r = r->right;
            else
                r = r->left;
        }
        if (!r)
            return nullptr;
        return r;
    }
    //取当前子树最小值
    RBNode* getMin(RBNode* t) {
        RBNode* res = nullptr;
        while (t) {
            res = t;
            t = t->left;
        }
        return res;
    }
    //取当前节点的后继节点，即第一个比当前节点大的节点
    RBNode* getNext(RBNode* t) {
        if (t && t->right)//右子树存在，直接从右子树里取最小值
        {
            return getMin(t->right);
        }
        else//右子树不存在，t往上取母树
        {
            while (t && t->p && t->p->left == t) {//这一段存疑
                t = t->p;
            }
            if (t && t->p)
                return t->p;
            else
                return nullptr;
        }
    }
    //删除调整
    void dkeep(RBNode* x, RBNode* px) {
        while (x != root && (!x || x->color == BLACK)) {
            if (x == px->left) {
                RBNode* w = px->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    px->color = RED;
                    leftR(px);
                    w = px->right;
                }
                if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK)) {
                    w->color = RED;
                    x = px;
                    px = px->p;
                }
                else {
                    if (!w->right || w->right->color == BLACK) {
                        w->color = RED;
                        w->left->color = BLACK;
                        rightR(w);
                        w = px->right;
                    }
                    w->color = px->color;
                    px->color = BLACK;
                    w->right->color = BLACK;
                    leftR(px);
                    x = root;
                }
            }
            else {
                RBNode* w = px->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    px->color = RED;
                    rightR(px);
                    w = px->left;
                }
                if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK)) {
                    w->color = RED;
                    x = px;
                    px = px->p;
                }
                else {
                    if (!w->left || w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        leftR(w);
                        w = px->left;
                    }
                    w->color = px->color;
                    px->color = BLACK;
                    w->left->color = BLACK;
                    rightR(px);
                    x = root;
                }

            }
        }
        x->color = BLACK;
    }
public:
    RBTree(RBNode* r = nullptr) :root(r) {}//构造函数
    //插入
    void insert(int key) {
        RBNode* tr = root;
        RBNode* ti = nullptr;
        while (tr) {
            ti = tr;
            if (tr->key < key)
                tr = tr->right;
            else
                tr = tr->left;
        }
        if (!ti)//当前红黑树为空，直接生成
            root = new RBNode(key,BLACK);
        else{
            RBNode* tokeep = new RBNode(key, RED, ti);//新加入节点颜色为红
            if (ti->key < key)
                ti->right = tokeep;
            else
                ti->left = tokeep;
            keep(tokeep);//调整
        }
    }
    //查找
    bool find(int key) {
        return getKey(key) != nullptr;
    }
    //删除
    void remove(int key) {
        RBNode* r = getKey(key);
        int color;
        RBNode* x = nullptr;
        RBNode* px = nullptr;
        if (!r)
            return;
        color = r->color;
        //叶子节点，直接删除值，如果无父节点，freeTree，如果有父节点，清空子树为nullptr
        if (!r->left && !r->right) {
            x = nullptr;
            px = r->p;
            if (!px) {
                root = nullptr;
                free(r);
                return;
            }
            else {
                if (px->left == r) {
                    px->left = x;
                }
                else {
                    px->right = x;
                }
            }
        }
        //要删除节点无左子树，将右子树接上父节点
        else if (!r->left) {
            x = r->right;
            px = r->p;
            if (!px) {
                root = x;
            }
            else {
                if (px->right == r) {
                    px->right = x;
                }
                else {
                    px->left = x;
                }
            }
            x->p = px;
        }
        //同上对称
        else if (!r->right) {
            x = r->left;
            px = r->p;
            if (!px) {
                root = x;
            }
            else {
                if (px->right == r) {
                    px->right = x;
                }
                else {
                    px->left = x;
                }
            }
            x->p = px;
        }
        //同时有左右子树的情况，逻辑比较复杂，暂时没理清，待整理
        else {
            RBNode* nr = getMin(r->right);	//nr->left==nullptr
            color = nr->color;				// nr->p != nullptr
            x = nr->right;
            px = nr->p;
            if (px->left == nr) {
                px->left = x;
            }
            else {
                px->right = x;
            }
            if (x) {
                x->p = px;
            }
            if (px == r)
                px = nr;
            if (!r->p) {
                root = nr;
            }
            else if (r->p->left == r) {
                r->p->left = nr;
            }
            else {
                r->p->right = nr;
            }
            nr->p = r->p;
            nr->left = r->left;
            nr->left->p = nr;
            nr->right = r->right;
            if (nr->right)
                nr->right->p = nr;

        }
        free(r);
        if (color == BLACK) {
            dkeep(x,px);
        }
    }
};
ostream& operator<<(ostream& o,const RBTree &t) {
    t.pr(o,t.root) << endl;
    t.mr(o,t.root) << endl;
    t.er(o,t.root) << endl;
    return o;
}

int main() {
    RBTree t;
    t.insert(1);
    cout << t << endl;
    t.insert(2);
    cout << t << endl;
    t.insert(3);
    cout << t << endl;
    t.insert(4);
    cout << t << endl;
    t.insert(8);
    cout << t << endl;
    t.insert(5);
    cout << t << endl;
    t.insert(6);
    cout << t << endl;
    t.insert(7);
    cout << t << endl;
    t.remove(3);
    cout << t << endl;

    return 0;
}

/*************************************************************************
	> File Name: RedBlackTree.h
	> Author: ZhirunZheng 
	> Mail: jiangxizhengzhirun@163.com 
	> Created Time: 2018年11月26日 星期一 19时41分54秒
 ************************************************************************/

#ifndef _REDBLACKTREE_H
#define _REDBLACKTREE_H
#include <stdexcept>
#include "../RedBlackTreeNode/RedBlackTreeNode.h"
// RedBlackTree: 红黑树 算法导论第13章
/*
 * 由于没有采用书中所述的带哨兵的红黑树,所以边界条件的处理比较复杂.
 * 红黑树是许多平衡搜索树中的一种,能够保证动态操作在最坏的情况下的时间复杂度为O(logN).
 * 红黑树的性质: 
 *      --1.每个节点或是红色的,或是黑色的.
 *      --2.根节点是黑色的.
 *      --3.每个叶节点(NULL)是黑色的.(特指为空的叶子节点)
 *      --4.如果一个节点是红色的,则它的两个子节点都是黑色的.
 *      --5.对每个节点,从该节点到其所有后代叶节点的简单路径上,均包含相同数目的黑色节点.
 *          (也就是黑高相同)
 * 与普通二叉搜索树不同的是,红黑树在每个节点上增加了一个存储位来表示节点的颜色(只能是BLACK
 *      和RED中的一种).通过对各个节点上颜色进行约束,可以保证任何一条从根到叶子节点的简单路径
 *      上不会比其它路径长2倍(这就保证了平衡).
 * 
 * 引理13.1 一棵有n个内部节点的红黑树的高度至多为2lg(n+1).
 *
 * 由该引理就可以知道,动态集合操作SEARCH, MINIMUM, MAXIMUM, SUCCESSOR和PREDECESSOR可在红黑树上
 * 在O(lgn)时间内执行.
 *
 * INSERT和DELETE操作在红黑树上的在O(lgn)时间内执行.
 */
template<typename NodeType>
class RedBlackTree
{
public:
    //***************************构造函数*********************************
    typedef typename NodeType::KeyType KeyType;     // 红黑树节点存储的数据类型
    RedBlackTree(): root(std::shared_ptr<NodeType>()) {  } // 默认构造函数
    ~RedBlackTree() = default;  // 析构函数
    //***************************成员函数*********************************
    void left_rotate(std::shared_ptr<NodeType>, std::shared_ptr<NodeType>&); // 红黑树左旋转操作
    void right_rotate(std::shared_ptr<NodeType>, std::shared_ptr<NodeType>&); // 红黑树右旋转操作
    void insert_fixup(std::shared_ptr<NodeType>);   // 在插入数据的时候保持红黑性质
    void insert(std::shared_ptr<NodeType>);     // 红黑树的插入操作
    void delete_fixup(std::shared_ptr<NodeType>);   // 在删除数据的时候保持红黑性质
    void transplant(std::shared_ptr<NodeType> ,std::shared_ptr<NodeType>); // 红黑树的剪切操作
    void remove(std::shared_ptr<NodeType>);     // 红黑树的删除操作
    std::shared_ptr<NodeType> minimum(std::shared_ptr<NodeType>);   // 最小关键值元素
    std::shared_ptr<NodeType> maximum(std::shared_ptr<NodeType>);   // 最大关键值元素
    std::shared_ptr<NodeType> successor(std::shared_ptr<NodeType>);     // 后继节点
    std::shared_ptr<NodeType> predecessor(std::shared_ptr<NodeType>);   // 前驱节点
    //***************************数据结构*********************************
    std::shared_ptr<NodeType> root;     // 红黑树的根节点
};
//**********************************成员函数******************************
// left_rotate: 红黑树的左旋转操作
/*
 * \parameter node: 待旋转的节点,强引用;
 * \parameter root: 红黑树根节点的强引用(当node指向树根的时候,旋转会导致丢失根的信息,此时root返回
 *                  新的树根);
 * \return void.
 *
 * 算法性能: 时间复杂度为O(1), 在旋转操作中只有指针改变,其他属性都保持不变.
 *
 * 算法基本思想: 本函数执行红黑树进行左旋转.设node为被旋转的点,l_node为它的左子节点,r_node为它的右子节点.
 *      --则旋转的效果就是:r_node取代了node的位置;而node现在挂靠在r_node的左子;r_node原来的左子现在成为
 *      --node的右子.
 *      |                                                  |
 *     node                                              r_node
 *    /    \                                            /      \
 * l_node  r_node            --左旋转 -->             node    r_r_node
 *        /      \                                   /    \
 *    l_r_node  r_r_node                           l_node l_r_node 
*/  
template<typename NodeType>
void RedBlackTree<NodeType>::left_rotate(std::shared_ptr<NodeType> node, 
                                         std::shared_ptr<NodeType> &root)
{
    // node为一个空指针
    if (!node){
        throw std::invalid_argument("node is nullptr!!!");
        return;
    }
    auto r_node = node->rchild;
    // r_node不是一个空指针
    if (r_node){
        auto l_r_node = r_node->lchild;
        // r_node上移
        r_node->parent = node->parent;
        auto shared_p = node->parent.lock();     // 取出node的父节点
        if (shared_p){
            if (node->is_left_child()) shared_p->lchild = r_node;
            if (node->is_right_child()) shared_p->rchild = r_node;
        }
        else
            root = r_node;
        node->rchild = l_r_node;
        if (l_r_node) l_r_node->parent = node;
        node->parent = r_node;
        r_node->lchild = node;
    }
}
// right_rotate: 红黑树的右旋转操作
/*
 * \parameter node: 待旋转的右节点,强引用;
 * \parameter root: 红黑树的根节点的强引用(当node指向树根时,旋转会导致丢失根的信息,此时root返回新的树根);
 * \return void.
 * 
 * 算法性能: 时间复杂度为O(1), 在旋转操作中只有指针改变,其它属性都保持不变.
 *
 * 算法的基本思想: 本函数执行红黑树进行右旋转.设node为被旋转的点,l_node是它的左子节点,r_node是它的右子节点.
 *          --则旋转的效果就是:l_node取代了node的位置;而node现在挂靠在r_node的右子;l_node原来的右子现在成为
 *          --node的左子.
 *            |                                             |
 *           node                                         l_node
 *          /    \                                       /      \ 
 *      l_node   r_node      --右旋转 -->           l_l_node   node
 *      /    \                                                 /   \
 *  l_l_node r_l_node                                    r_l_node  r_node
*/
template<typename NodeType>
void RedBlackTree<NodeType>::right_rotate(std::shared_ptr<NodeType> node, 
                                          std::shared_ptr<NodeType> &root)
{
    if (!node){
        throw std::invalid_argument("node is nullptr!!!");
        return;
    }
    auto l_node = node->lchild;
    // l_node不是一个空指针
    if (l_node){
        auto r_l_node = l_node->rchild;
        l_node->parent = node->parent;
        auto shared_p = node->parent.lock();    // 取出父节点
        if (shared_p){
            if (node->is_left_child()) shared_p->lchild = l_node;
            if (node->is_right_child()) shared_p->rchild = l_node;
        }
        else
            root = l_node;
        node->lchild = r_l_node;
        if (r_l_node) r_l_node->parent = node;
        node->parent = l_node;
        l_node->rchild = node;
    }
}
// insert_fixup: 插入操作保持红黑性质
/*
 * \parameter node: 新插入的节点;
 * \return void.
 * 红黑树的性质: 
 *      --1.每个节点或是红色的,或是黑色的.
 *      --2.根节点是黑色的.
 *      --3.每个叶节点(NULL)都是黑色的.(特指为空的叶子节点).
 *      --4.如果一个节点是红色的,则它的两个子节点都是黑色的.
 *      --5.对每个节点,从该节点到其他所有后代叶节点的简单路径上,均包含相同数目的黑色节点.
 * 当插入一个节点(节点颜色为红色)红黑树的性质1和性质3以及性质5继续成立,唯有可能破坏的是性质2和性质4.
 *                      
 * 算法性能: 算法时间复杂度为O(logN).所有的旋转操作都不会超过两次.
 *  0:红色;  1:黑色;
 *                  |                                                        |min = normalTree.minimum(normalTree.root);
 *                11<1>                                                    11<1>
 *               /      \                                                 /     \
 *             2<0>      14<1>                                          2<0>    14<1><uncle>
 *            /    \          \            --情况1-->                  /    \           \
 *          1<1>   7<1>      15<0>                                   1<1> 7<0><node>   15<0>
 *                /    \                                                    /   \
 *              5<0>   8<0><uncle>                                        5<1>  8<1>
 *             /                                                         /
 *       4<0><node>                                                     4<0>
 *
 * --情况2-->(左旋转操作)                 
 *                  |                                                          | 
 *                11<1>                                                       7<1> 
 *               /     \                                                     /    \
 *            7<0>     14<1><uncle>                                  2<0><node>    11<0>
 *           /    \        \            --情况3-->(右旋转操作)        /       \      /  \
 *   2<0><node>   8<1>    15<0>                                     1<1>      5<1> 8<1>  14<1>
 *       /    \                                                               /              \
 *     1<1>   5<1>                                                           4<0>            15<0>
 *            /
 *           4<0>
*/
template<typename NodeType>
void RedBlackTree<NodeType>::insert_fixup(std::shared_ptr<NodeType> node)
{
    auto node_p = node->parent.lock();     // 取出node节点的父节点
    // 父节点为空
    if (!node_p){
        root = node;
        node->color = BLACK;
        return;
    }
    // 违反了性质4(进行修改)
    while (node_p->color == RED){
        // 能进入这个循环暗含节点node的祖父节点一定存在
        auto uncle = std::shared_ptr<NodeType>();   // 叔叔节点
        auto node_p_p = node_p->parent.lock();
        if (node_p->is_left_child()){
            uncle = node_p_p->rchild;
            if (uncle && uncle->color == RED){      // case1 叔节点为红色
                node_p->color = BLACK;  // 父节点涂黑
                uncle->color = BLACK;   // 叔节点涂黑
                node_p_p->color = RED;  // 祖父节点涂红
                node = node_p_p;    // 提升节点node的位置
                node_p = node->parent.lock();   // 修改父节点
                if (!node_p)    // 边界处理
                    node->color = BLACK;
            }
            // case2叔节点为黑色且node为双亲的右孩子(当叔节点不存在的时候默认为黑色
            else if(node->is_right_child()){  
                node = node_p;  
                left_rotate(node, root);  // 左旋转操作
                node_p = node->parent.lock();   // 修改父节点
                node_p_p = node_p->parent.lock();   // 修改祖父节点
                node_p->color = BLACK;  // case2 转为case3处理
                node_p_p->color = RED;  // case2 转为case3处理
                right_rotate(node_p_p, root);  // 右旋转操作
            }
            else{
                node_p->color = BLACK;  // 父节点涂黑
                node_p_p->color = RED;  // 祖父节点涂红
                right_rotate(node_p_p, root);
            }
        }
        // 对称处理
        else{
            uncle = node_p_p->lchild;     // 拿出叔节点
            if (uncle && uncle->color == RED){      // case1 叔节点为红色
                node_p->color = BLACK;  // 父节点涂黑
                uncle->color = BLACK;   // 叔节点涂黑
                node_p_p->color = RED;  // 祖父节点涂红
                node = node_p_p;
                node_p = node->parent.lock();  // 修改父节点
                if (!node_p)    // 边界处理
                    node->color = BLACK;
            }
            // case2叔节点为黑色且node为双亲的左孩子(当叔节点不存在的时候默认为黑色)
            else if(node->is_left_child()){  
                node = node_p;
                right_rotate(node, root);   // 右旋转操作
                node_p = node->parent.lock();   // 修改父节点
                node_p_p = node_p->parent.lock(); // 修改祖父节点
                node_p->color = BLACK;      // case2转为case3处理
                node_p_p->color = RED;      // case2转为case3处理
                left_rotate(node_p_p, root);    // 左旋转操作
            }
            else{
                node_p->color = BLACK;  // 父节点涂黑
                node_p_p->color = RED;  // 祖父节点涂红
                left_rotate(node_p_p, root); // 左旋转操作
            }
        }
        if (!node_p)
            break;
    }
}
// insert: 红黑树的插入操作
/*
 * \parameter node: 待插入的元素;
 * \return void.
 *
 * 算法基本思想: 遍历红黑树,若当前节点的值大于等于'node'的值,则向右侧遍历;
 *          若当前的值小于等于'node'的值,则向左遍历.直到碰到'nullptr'则挂载该节点.
 *          最后将'node'着为红色,为了保证红黑树的性质能够继续保持,调用一个辅助程序
 *          insert_fixup(node)来对节点重新着色并旋转.
 * 算法性能: 算法的时间复杂度为O(logn).
*/
template<typename NodeType>
void RedBlackTree<NodeType>::insert(std::shared_ptr<NodeType> node)
{
    if (!root){
        root = node;
        return;
    }
    auto temp = root;
    bool left = true;
    auto temp_parent = std::shared_ptr<NodeType>();
    while (temp){
        temp_parent = temp;
        if (node->key < temp->key){
            temp = temp->lchild;
            left = true;
        }
        else{
            temp = temp->rchild;
            left = false;
        }
    }
    // 现在temp为空,把node挂靠在temp->parent下面
    node->parent = temp_parent;
    if (left) temp_parent->lchild = node;
    else temp_parent->rchild = node;
    node->color = RED;
    insert_fixup(node);     // 保持红黑性质
}
// delete_fixup: 删除操作保持红黑性质
/*
 * \parameter node: 待删除的节点;
 * \return void.
 * 
 * 算法性能: 算法的时间复杂度为O(logN).
 *
 * 修正过程分为四种情况:
 * 情况1: x的右兄弟w是红色的,说明x的父节点一定是黑色的.
 *      --所做的操作是:交换w和其父节点的颜色,即把w换为黑色,其父节点换为红色;
 *      --然后对父节点左旋转,w重新指向x的右兄弟(该节点原本是w的左孩子,所以一定为黑色).
 *      --这时情况1就转变为情况2.
 *          |                                                       |
 *         B<1>                                                    D<1>
 *        /    \                  情况1                           /    \
 *   x:A<1>    w:D<0>      ------------------->                B<0>    E<1>
 *            /      \                                         /   \
 *         C<1>      E<1>                                 x:A<1>   new w:C<1>
 * 情况2: x的兄弟节点w是黑色的,而且w的孩子都为黑色.
 *      --所做的操作是: 将w换为红色,x指向其父节点
 *          |                                                       | 
 *         B<1>                                                 new x:B<1>
 *        /    \                  情况2                         /         \
 *   x:A<1>    w:D<1>      ------------------->              A<1>         D<0>
 *             /     \                                                    /   \
 *            C<1>   E<1>                                              C<1>   E<1>
 * 情况3: x的兄弟节点w是黑色的,w的左孩子是红色的,w的右孩子是黑色的.
 *      --所做的操作是:交换w和其左孩子的颜色,即把w换为红色,其左孩子换为黑色;
 *      --然后对w右旋转,w重新指向x的右兄弟;
 *          |                                                       |  
 *         B<0>                                                    B<1>  
 *        /    \                  情况3                            /   \
 *    x:A<1>   w:D<1>      ------------------->               x:A<1>   new w: C<1>
 *             /     \                                                    \
 *           C<0>    E<1>                                                 D<0>
 *                                                                          \
 *                                                                          E<1>
 * 情况4: x的兄弟节点w是黑色的,且w的右孩子是红色的.
 *      --所做的操作是:w与x的父节点交换颜色;
 *      --并把w的右孩子设为黑色,对x的父节点左旋转,x直接指向根节点,循环结束.
 *          |                                                       |
 *        B<0>                                                     D<0>
 *       /    \                   情况4                           /    \
 *  x:A<1>    w:D<1>       ------------------->                B<1>    E<1>
 *            /     \                                         /    \
 *           C<0>   E<0>                                     A<1>  C<0>  注:new x = root. 
*/
template<typename NodeType>
void RedBlackTree<NodeType>::delete_fixup(std::shared_ptr<NodeType> node)
{
    std::shared_ptr<NodeType> w = std::shared_ptr<NodeType>();
    while (node != root && node && node->color == BLACK){
        if (node->is_left_child()){     // node是其父亲的左孩子
            auto node_p = node->parent.lock();  // 提取node的父亲节点
            w = node_p->rchild;
            // 情况1: x的兄弟节点w是红色的(当w不存在的时候默认为黑色)
            if (w && w->color == RED){
                w->color = BLACK;
                node_p->color = RED;
                left_rotate(node_p, root);
                w = node_p->rchild;
            }
            // 情况2: x的兄弟节点w是黑色的,而且w的两个子节点都是黑色的   
            // 边界处理的条件: w存在且为黑色
            if (w && w->color == BLACK){
                w->color = RED;
                node = node->parent.lock();
            }
            // 情况3: x的兄弟节点w是黑色的,w的左孩子是红色的,w的右孩子是黑色的
            // 边界条件的处理: w存在且为黑色 && w的左孩子存在且为红色 && (w的右孩子不存在 || w的有孩子存在
            // 且为黑色) 
            else if ((w && w->color == BLACK) && (w->lchild && w->lchild->color == RED) &&
                    (!w->rchild || (w->rchild && w->rchild->color == BLACK))){
                // 转为情况4
                w->lchild->color = BLACK;
                w->color = RED;
                right_rotate(w, root);
                node_p = node->parent.lock();
                w = node_p->rchild;
                /*
                // 情况4的处理过程
                node_p = node->parent.lock();
                w = node_p->rchild;
                w->color = node_p->color;
                node_p->color = BLACK;
                w->rchild->color = BLACK;
                left_rotate(node_p, root);
                node = root;
                */
            }
            // 情况4: x的兄弟节点w是黑色的,且w的右孩子是红色的
            // 边界条件的处理: 1. w存在且为黑色 && w的右孩子存在且为红色
            else if ((w && w->color == BLACK) && (w->rchild && w->rchild->color == RED)){
                node_p = node->parent.lock();
                w->color = node_p->color;
                node_p->color = BLACK;
                w->rchild->color = BLACK;
                left_rotate(node_p, root);
                node = root;
            }
        }
        else{
            auto node_p = node->parent.lock();
            w = node_p->lchild;
            // 情况1:x的兄弟节点w是红色的(当x不存在的时候默认为黑色)
            if (w && w->color == RED){
                w->color = BLACK;
                node_p->color = RED;
                right_rotate(node_p, root);
            }
            // 情况2:x的兄弟节点是黑色的,并且w的两个子节点都是黑色的
            if (w && w->color == BLACK){
                w->color = RED;
                node = node->parent.lock();
            }
            // 情况3:x的兄弟节点w是黑色的,w的左孩子是黑色的,w的右孩子是红色的
            // 边界条件的处理: w存在且为黑色 && w的右孩子存在且为红色 && (w的左孩子不存在 ||
            // w的左孩子存在且为红色且为黑色)
            else if ((w && w->color == BLACK) && (w->rchild && w->rchild->color == RED) &&
                    (!w->lchild || (w->lchild && w->lchild->color == BLACK))){ 
                // 转化为情况4
                w->rchild->color = BLACK;
                w->color = RED;
                left_rotate(w, root);
                node_p = node->parent.lock();
                w = node_p->lchild;
                /*
                // 情况4的处理
                node_p = node->parent.lock();
                w = node_p->lchild;
                w->color = node_p->color;
                node_p->color = BLACK;
                w->lchild->color = BLACK;
                right_rotate(node_p, root);
                node = root;
                */
            }
            // 情况4: x的兄弟节点w是黑色的,且w的左孩子是红色的
            // 边界条件的处理: w存在且为黑色 && w的左孩子存在且为红色
            else if ((w && w->color == BLACK) && (w->lchild && w->lchild->color == RED)){
                node_p = node->parent.lock();
                w->color = node_p->color;
                node_p->color = BLACK;
                w->lchild->color = BLACK;
                right_rotate(node_p, root);
                node = root;
            }
        }
    }
    if (node)
        node->color = BLACK;
}
// transplant: 红黑树的剪切操作
/*
 * \parameter u: 用一棵以v为根的子树替换一棵以u为根的子树;
 * \parameter v: 用一棵以v为根的子树替换一棵以u为根的子树;
 * \return void.
 *
 * 算法基本思想: 用一棵以v为根的子树替换一棵以u为根的子树时,节点v的双亲就变成节点u的双亲,并且
 *      最后v成为u的双亲相应的孩子.
*/
template<typename NodeType>
void RedBlackTree<NodeType>::transplant(std::shared_ptr<NodeType> u, std::shared_ptr<NodeType> v)
{
    if (!u)
        throw std::invalid_argument("node is nullptr!!!");
    auto u_parent = u->parent.lock();   // 提取父节点
    if (!u_parent)     //u为根节点
        root = v;
    else if (u->is_left_child())    //u为左孩子节点
        u_parent->lchild = v;
    else if (u->is_right_child())   //u为右孩子节点
        u_parent->rchild = v;
    if (v)      //v非空
        v->parent = u_parent;
}
// remove: 红黑树的删除操作
/*
 * \parameter node: 待删除的节点;
 * \return void.
 * 
 * 算法性能: 时间复杂度为O(logN).
 *
 * 算法的基本思想: 和二叉搜索树的删除操作一样,同样分为:少于两个子节点,有两个子节点的情况.
 *      --1.node的子节点数少于两个:(令y指向node的位置)如果节点y颜色为红色直接删除,因为不会破坏红黑性质;
 *          若y为黑色,调用颜色修复函数,并令其子树x代替node的位置,并把颜色也改变成node的颜色.
 *      --2.node的子节点数有两个: 找到node的后继y,用y代替node的位置,并把y的颜色换成node的颜色,这样不会
 *          破坏红黑性质.但是如果y在之前的位置是黑色的,现在由于转移走了,y的右子树x代替了y的位置,此时破坏了
 *          这个支树的红黑性质,少了一个黑色节点,需要调用颜色修复函数.
*/
template<typename NodeType>
void RedBlackTree<NodeType>::remove(std::shared_ptr<NodeType> node)
{
    if (!node)
        throw std::invalid_argument("node remove is nullptr!!!");
    // 判定'node'必须在树中
    auto temp = root;
    while (temp && temp.get() != node.get()){
        if (node->key < temp->key)
            temp = temp->lchild;
        else
            temp = temp->rchild;
    }
    if (temp.get() != node.get())
        throw std::invalid_argument("node removed must be in tree!!!");
    // 删除过程
    std::shared_ptr<NodeType> x = std::shared_ptr<NodeType>();
    auto y = node;
    COLOR y_color = RED;    // 记录y转移前节点的颜色
    
    // node是一个叶子节点(直接删除)
    if ((!node->lchild) && (!node->rchild)){
        auto parent_ptr = node->parent.lock();  // 获取父节点
        // 父节点非空
        if (parent_ptr){
            if (node->is_left_child())
                parent_ptr->lchild = std::shared_ptr<NodeType>();
            if (node->is_right_child())
                parent_ptr->rchild = std::shared_ptr<NodeType>();
        }
        // 父节点为空(说明这个节点为根节点 且这棵树只有这一个节点)
        else
            root = std::shared_ptr<NodeType>();
    }
    else if (node->rchild && !node->lchild){     // 右子树存在,左子树不存在
        x = node->rchild;    
        transplant(node, node->rchild);
    }
    else if (node->lchild && !node->rchild){    // 左子树存在,右子树不存在
        x = node->lchild;
        transplant(node, node->lchild);
    }
    // 节点node的左子树和右子树都存在
    else{
        y = successor(node);    // 找到node节点的后继节点
        x = y->rchild;
        y_color = y->color;     // 记录y转移前的颜色
        // node的后继节点y不是node的右子节点(但一定存在于右子树中),那么转换为右子节点的情形
        if (y != node->rchild){
            transplant(y, y->rchild);   // 将后继的右子剪切到后继的位置
            y->rchild = node->rchild;
            node->rchild->parent = y;
            node->rchild = y;
        }
        // node的后继就是node的右子节点
        transplant(node, y);
        y->lchild = node->lchild;
        node->lchild->parent = y;
        y->color = node->color;     // 将y节点的颜色换成node的颜色
    }
    // 转移前节点y的颜色为黑色的话那么就必须要调用颜色修复函数(维持红黑树的性质)
    
    if (y_color == BLACK)
        delete_fixup(x);
}
// minimum: 最小关键值元素
/*
 * \parameter node: 求以node为节点的子树的最小关键值元素;
 * \return 返回指向最小关键值元素的强引用.
 * 算法基本思想: 通过从树根开始,沿着左孩子指针直到遇到一个'nullptr'.
 *          红黑树的性质保证了这样做能找到树中的最小值.
 * 算法性能: 算法时间复杂度为O(logN).
*/
template<typename NodeType>
std::shared_ptr<NodeType> RedBlackTree<NodeType>::minimum(std::shared_ptr<NodeType> node)
{
    auto current = node;
    auto current_parent = std::shared_ptr<NodeType>();
    while (current){
        current_parent = current;
        current = current->lchild;
    }
    if (!current_parent)
        std::cerr << "这是一棵空树!!!" << std::endl;
    return current_parent;
}
// maximum: 最大关键值元素 
/*
 * \parameter node: 求以node为节点的子树的最大关键字元素;
 * \return 返回指向通过最大关键值元素的强引用.
 * 算法的基本思想: 通过从树根开始,沿着右孩子指针直到遇到一个'nullptr'.
 *          红黑树的性质保证了这样做能找到树中的最大值.
 * 算法性能: 算法时间复杂度为O(logN).
*/
template<typename NodeType>
std::shared_ptr<NodeType> RedBlackTree<NodeType>::maximum(std::shared_ptr<NodeType> node)
{
    auto current = node;
    auto current_parent = std::shared_ptr<NodeType>();
    while (current){
        current_parent = current;
        current = current->rchild;
    }
    if (!current_parent)
        std::cerr << "这是一棵空树!!!" << std::endl;
    return current_parent;
}
// successor: 红黑树的后继节点
/*
 * \parameter node: 求取节点node的后继节点;
 * \return 指向后继节点的强引用,或者为空.
 *
 * 给定红黑树的某个节点,搜索其后继节点.所谓的某节点'node'的后继节点就是在红黑树中,值大于等于'node'的所有
 * 节点中最小的那一个(排除自己).
 *
 * 一个节点'node'的后继有以下情况:
 *      --如果'node'有右子节点,则以右子节点为根的子树中最小值节点就是'node'的后继节点;
 *      --如果'node'没有右子节点,则看父节点
 *          --若'node'是父节点的左子节点,则'node'的后继节点就是'node'的父节点;
 *          --若'node'是父节点的右子节点,则'node'设置为'node->parent',不断循环到'node'是它的父亲的左子节点;
 *            此时'node'的后继节点是'node'的父节点.
 *      --注: 如果'node'是这棵树中的最大关键值,则返回'nullptr'.
 * 算法时间复杂度为:O(h),空间复杂度为O(1).其中h为树的高度.
*/
template<typename NodeType>
std::shared_ptr<NodeType> RedBlackTree<NodeType>::successor(std::shared_ptr<NodeType> node)
{
    if (!node)
        throw std::invalid_argument("successor() should not be aplied on nullptr!!!");
    // 以右子节点为根的子树中的最小值就是'node'的后继节点
    if (node->rchild)
        return minimum(node->rchild);
    auto shared_p = node->parent.lock();    // 取出node的父亲节点
    auto current = node;
    while (shared_p && current->is_right_child()){
        current = shared_p;
        shared_p = current->parent.lock();
    }
    return shared_p;
}
// predecessor: 红黑树的前驱节点
/*
 * \parameter node: 求取节点node的前驱节点;
 * \return 指向前驱节点的强引用,或者为空.
 *
 * 给定红黑树的某个节点,搜索其前驱节点.所谓节点'node'的前驱节点就是在红黑树中,值小于等于'node'的所有节点
 * 中最大的那一个(排除自己).
 *
 * 一个节点'node'的前驱有以下情况:
 *      --如果'node'有左子节点,则以左子节点为根的子树中最大值节点就是'node'的前驱节点;
 *      --如果'node'没有左子节点,则看父节点
 *          --若'node'是父节点的右子节点;则'node'的前驱节点就是'node'的父节点;
 *          --若'node'是父节点的左子节点;则'node'设置为'node->parent',不断循环到'node'是它的父亲的右子节点;
 *            此时'node'的前驱节点就是'node'的父节点.
 *      --注: 如果'node'是这棵树中的最小关键值,则返回'nullptr'.
 * 算法时间复杂度为: O(h), 空间复杂度为O(1). 其中h为树的高度.
*/
template<typename NodeType>
std::shared_ptr<NodeType> RedBlackTree<NodeType>::predecessor(std::shared_ptr<NodeType> node)
{
    if (!node)
        throw std::invalid_argument("predecessor() should not be aplied on nullptr!!!");
    // 以左子节点为根的子树中的最大值节点就是'node'的前驱节点
    if (node->lchild)
        return maximum(node->lchild);
    auto shared_p = node->parent.lock();    // 取出node的父节点
    auto current = node;
    while (shared_p && current->is_left_child()){
        current = shared_p;
        shared_p = shared_p->parent.lock();
    }
    return shared_p;
}
#endif

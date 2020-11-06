#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;

template <class T>
class IList
{
public:
    virtual void add(const T& element) = 0;
    virtual void add(int index, const T& element) = 0;
    virtual T removeAt(int index) = 0;
    virtual bool removeItem(const T& item) = 0;
    virtual bool empty() = 0;
    virtual int size() = 0;
    virtual void clear() = 0;
    virtual T get(int index) = 0;
    virtual void set(int index, const T& element) = 0;
    virtual int indexOf(const T& item) = 0;
    virtual bool contains(const T& item) = 0;
};
template <class T>
class FragmentLinkedList : IList<T>
{
public:
    class Node;
    class Iterator;

protected:
    Node **fragmentPointers;
    int fragmentMaxSize;
    int count;

public:
    FragmentLinkedList(int fragmentMaxSize = 5)
    {
        this->count = 0;
        this->fragmentMaxSize = fragmentMaxSize;
        this->fragmentPointers = new Node *[2];
        this->fragmentPointers[0] = NULL;
        this->fragmentPointers[1] = NULL;
    }
    virtual ~FragmentLinkedList();
    virtual void add(const T &element);
    virtual void add(int index, const T &element);
    virtual T removeAt(int index);
    virtual bool removeItem(const T &item);
    virtual bool empty();
    virtual int size();
    virtual void clear();
    virtual T get(int index);
    virtual void set(int index, const T &element);
    virtual int indexOf(const T &item);
    virtual bool contains(const T &item);
    Iterator begin(int index = 0)
    {

        if (index == 0)
        {
            return Iterator(this, true);
        }
        else if (index > 0 && index < this->numberOfPointer() - 1)
        {
            return Iterator(this, index, true);
        }
        throw std::out_of_range("The index is out of range!");
    }
    Iterator end(int index = -1)
    {

        if (index == -1)
        {
            return Iterator(this, false);
        }
        else if (index > -1 && index < this->numberOfPointer() - 1)
        {
            return Iterator(this, index, false);
        }
        throw std::out_of_range("The index is out of range!");
    }
    void makeFragment();
    int numberOfPointer();

public:
    class Node
    {
    private:
        T data;
        Node *next;
        Node *prev;
        friend class FragmentLinkedList<T>;

    public:
        Node()
        {
            next = 0;
            prev = 0;
        }
        Node(Node *next, Node *prev)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next, Node *prev)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    class Iterator
    {
    private:
        FragmentLinkedList<T> *pList;
        Node *pNode;
        int index;

    public:
        Iterator(FragmentLinkedList<T> *pList = 0, bool begin = true)
        {
            this->pList = pList;
            if (pList == NULL)
            {
                this->pNode = NULL;
                index = begin ? -1 : 0;
            }
            else
            {
                if (begin == true)
                {
                    this->pNode = pList->fragmentPointers[0];
                    index = 0;
                }
                else
                {
                    this->pNode = NULL;
                    index = pList->size();
                }
            }
        }
        Iterator(FragmentLinkedList<T> *pList = 0, int fragmentIndex = 0, bool begin = true)
        {
            if (fragmentIndex < 0 || (fragmentIndex >= pList->numberOfPointer() - 1))
            {
                throw std::out_of_range("The index is out of range!");
            }
            this->pList = pList;
            if (pList == NULL)
            {
                this->pNode = NULL;
                index = begin ? -1 : 0;
            }
            else
            {

                if (begin == true)
                {
                    this->pNode = pList->fragmentPointers[fragmentIndex];
                    index = pList->fragmentMaxSize * fragmentIndex;
                }
                else
                {
                    if (fragmentIndex != pList->numberOfPointer() - 2)
                    {
                        this->pNode = pList->fragmentPointers[fragmentIndex + 1];
                        index = pList->fragmentMaxSize * (fragmentIndex + 1);
                    }
                    else
                    {
                        this->pNode = NULL;
                        index = pList->size();
                    }
                }
            }
        }
        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            this->index = iterator.index;
            return *this;
        }
        T &operator*()
        {
            if (pNode != NULL)
            {
                return pNode->data;
            }
            throw std::out_of_range("Segmentation fault!");
        }
        bool operator!=(const Iterator &iterator)
        {
            return this->pNode != iterator.pNode || this->index != iterator.index;
        }
        void remove()
        {
            if (pNode == NULL)
            {
                throw std::out_of_range("Segmentation fault!");
            }
            if (pNode == pList->fragmentPointers[0])
            {
                Node *p = pList->fragmentPointers[0];
                pList->fragmentPointers[0] = pList->fragmentPointers[0]->next;
                if (pList->fragmentPointers[0] != NULL)
                    pList->fragmentPointers[0]->prev = NULL;
                pNode = NULL;
                index = -1;
                delete p;
            }
            else if (pNode == pList->fragmentPointers[pList->numberOfPointer() - 1])
            {
                int indexOfTailPointer = pList->numberOfPointer() - 1;
                Node *p = pList->fragmentPointers[indexOfTailPointer];
                pList->fragmentPointers[indexOfTailPointer] = pList->fragmentPointers[indexOfTailPointer]->prev;
                if (pList->fragmentPointers[indexOfTailPointer] != NULL)
                    pList->fragmentPointers[indexOfTailPointer]->next = NULL;
                pNode = pList->fragmentPointers[indexOfTailPointer];
                index--;
                delete p;
            }
            else
            {
                Node *prevPointer = pNode->prev;
                Node *nextPointer = pNode->next;
                Node *deletedPointer = pNode;
                prevPointer->next = nextPointer;
                nextPointer->prev = prevPointer;
                pNode = prevPointer;
                index--;
                delete deletedPointer;
            }
            pList->count--;
            pList->makeFragment();
        }
        void set(const T &element)
        {
            if (pNode == NULL)
                throw std::out_of_range("Segmentation fault!");
            pNode->data = element;
        }
        Iterator &operator++()
        {
            if (index == pList->size())
            {
                throw std::out_of_range("Segmentation fault!");
            }
            else if (pNode == NULL && index == -1)
            {
                pNode = pList->fragmentPointers[0];
            }
            else if (pNode != NULL)
            {
                pNode = pNode->next;
            }
            index++;
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
};
template <class T>
void FragmentLinkedList<T>::set(int index, const T &element)
{
    if (this->count != 0 && index < this->count && index >= 0)
    {
        this->removeAt(index);
        this->add(index, element);
        return;
    }
    throw std::out_of_range("The index is out of range!");
}
template <class T>
T FragmentLinkedList<T>::removeAt(int index)
{
    if (this->count == 0)
    {
        throw std::out_of_range("The index is out of range!");
    }
    if (index == 0)
    {
        Node *head = fragmentPointers[0];
        fragmentPointers[0] = head->next;
        if (fragmentPointers[0] != NULL)
            fragmentPointers[0]->prev = NULL;
        head->prev = NULL;
        head->next = NULL;
        this->count--;
        this->makeFragment();
        T valueRemoved = head->data;
        delete head;
        return valueRemoved;
    }
    else if (index > 0 && index < this->count)
    {
        Node *head = fragmentPointers[0];
        while (index--)
        {
            if (index == 0)
            {
                Node *temp = head->next;
                head->next = temp->next;
                Node *nextTemp = temp->next;
                if (nextTemp != NULL)
                    nextTemp->prev = head;
                temp->next = NULL;
                temp->prev = NULL;
                if (nextTemp == NULL)
                {
                    head->next = NULL;
                }
                this->count--;
                this->makeFragment();
                T valueRemoved = temp->data;
                delete temp;
                return valueRemoved;
            }
            else
            {
                head = head->next;
            }
        }
    }
    throw std::out_of_range("The index is out of range!");
}
template <class T>
bool FragmentLinkedList<T>::removeItem(const T &item)
{
    int indexOfItem = this->indexOf(item);
    if (indexOfItem != -1)
    {
        this->removeAt(indexOfItem);
        return true;
    }
    else
    {
        return false;
    }
}
template <class T>
int FragmentLinkedList<T>::indexOf(const T &item)
{
    if (fragmentPointers[0] == NULL)
        return -1;
    Node *temp = fragmentPointers[0];
    int index = 0;
    while (temp != NULL)
    {
        if (temp->data == item)
        {
            return index;
        }
        index++;
        temp = temp->next;
    }
    return -1;
}
template <class T>
bool FragmentLinkedList<T>::contains(const T &item)
{
    if (fragmentPointers[0] == NULL)
        return false;
    Node *temp = fragmentPointers[0];
    while (temp != NULL)
    {
        if (temp->data == item)
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}
template <class T>
int FragmentLinkedList<T>::numberOfPointer()
{
    int numberOfPointer = (this->count % fragmentMaxSize != 0) ? (this->count / fragmentMaxSize + 2) : (this->count / fragmentMaxSize + 1);
    if (this->count == 0)
    {
        numberOfPointer = 2;
    }
    return numberOfPointer;
}
template <class T>
void FragmentLinkedList<T>::makeFragment()
{
    if (this->count == 0)
    {
        delete[] fragmentPointers;
        fragmentPointers = NULL;
        this->fragmentPointers = new Node *[2];
        this->fragmentPointers[0] = NULL;
        this->fragmentPointers[1] = NULL;
        return;
    }
    int numberPoinList = this->numberOfPointer();
    Node *temp = fragmentPointers[0];

    delete[] fragmentPointers;
    fragmentPointers = new Node *[numberPoinList];
    int countIndex = 0;
    for (int i = 0; i < this->count; i++)
    {
        if (i % fragmentMaxSize == 0)
        {
            fragmentPointers[countIndex] = temp;
            countIndex++;
        }
        if (i != this->count - 1)
            temp = temp->next;
    }
    fragmentPointers[countIndex] = temp;
}
template <class T>
T FragmentLinkedList<T>::get(int index)
{
    Node *head = this->fragmentPointers[0];
    if (index == 0)
    {
        return head->data;
    }
    else if (index > 0 && index < this->count)
    {
        index++;
        while (index--)
        {
            if (index == 0)
            {
                return head->data;
            }
            else
            {
                head = head->next;
            }
        }
    }
    throw std::out_of_range("The index is out of range!");
}
template <class T>
void FragmentLinkedList<T>::add(int index, const T &element)
{
    if (index == this->count)
    {
        this->add(element);
        return;
    }
    else if (index == 0)
    {
        Node *newNode = new Node(element, this->fragmentPointers[0], NULL);
        this->fragmentPointers[0]->prev = newNode;
        this->fragmentPointers[0] = newNode;
        this->count++;
        this->makeFragment();
        return;
    }
    else if (index > 0 && index < this->count)
    {
        Node *head = this->fragmentPointers[0];
        while (index--)
        {

            if (index == 0)
            {
                Node *temp = new Node(element, NULL, NULL);
                temp->next = head->next;
                temp->next->prev = temp;
                head->next = temp;
                temp->prev = head;
            }
            else
            {
                head = head->next;
            }
        }
        this->count++;
        this->makeFragment();
        return;
    }
    throw std::out_of_range("The index is out of range!");
}

template <class T>
void FragmentLinkedList<T>::add(const T &element)
{
    if (this->count == 0)
    {
        this->fragmentPointers[0] = new Node(element, NULL, NULL);
        this->count++;
        this->makeFragment();
        return;
    }

    Node *head = this->fragmentPointers[0];
    while (head->next != NULL)
        head = head->next;
    Node *newNode = new Node(element, NULL, head);
    head->next = newNode;
    this->count++;
    this->makeFragment();
}
template <class T>
FragmentLinkedList<T>::~FragmentLinkedList()
{
    Node *head = this->fragmentPointers[0];
    Node *temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        delete temp;
    }
    delete[] fragmentPointers;
}
template <class T>
bool FragmentLinkedList<T>::empty()
{
    return this->count == 0;
}
template <class T>
int FragmentLinkedList<T>::size()
{
    return this->count;
}
template <class T>
void FragmentLinkedList<T>::clear()
{
    this->~FragmentLinkedList();
    this->count = 0;
    this->fragmentPointers = new Node *[2];
    this->fragmentPointers[0] = NULL;
    this->fragmentPointers[1] = NULL;
}

class BusSystem;
class ChuyenXe {
private:
    string maTuyen;   
    string bienKiemSoat;
    string thoiGianXuatBen;
    string thoiGianDenCuoi;
    friend class BusSystem;
public:
    ChuyenXe(string maTuyen = "", string bienKiemSoat = "", string thoiGianXuatBen = "", string thoiGianDenCuoi="") {
        this->maTuyen = maTuyen;
        this->bienKiemSoat = bienKiemSoat;
        this->thoiGianXuatBen = thoiGianXuatBen;
        this->thoiGianDenCuoi = thoiGianDenCuoi;
    }
    bool operator==(const ChuyenXe& rhs) const {
        return this->maTuyen == rhs.maTuyen && this->bienKiemSoat == rhs.bienKiemSoat;
    }
    string getMaTuyen() {
    	return maTuyen;
	}
	string getBienKiemSoat() {
		return bienKiemSoat;
	}
	string getthoiGianXuatBen() {
		return thoiGianXuatBen;
	}
	string getThoiGianDenCuoi() {
		return thoiGianDenCuoi;
	}
};
class BusSystem {
private:
    FragmentLinkedList<ChuyenXe> **conTroVaoDanhSachTuyen;
    int soChuyenToiDaCuaTuyen;
    int soTuyenHienTai;
public:
    BusSystem() {
        this->soChuyenToiDaCuaTuyen = 5;
        this->soTuyenHienTai = 0;
        conTroVaoDanhSachTuyen = new FragmentLinkedList<ChuyenXe>*();
    }
    ~BusSystem() {
        for (int i = 0; i < soTuyenHienTai; i++) {
        conTroVaoDanhSachTuyen[i]->clear();
    	}
   		delete[] conTroVaoDanhSachTuyen;
    }
//    void insertChuyenXe(const ChuyenXe& ChuyenXe);
    void themTuyen() {
    	soTuyenHienTai++;
    	FragmentLinkedList<ChuyenXe> **temp = conTroVaoDanhSachTuyen;
		conTroVaoDanhSachTuyen = new FragmentLinkedList<ChuyenXe> *[soTuyenHienTai];
		conTroVaoDanhSachTuyen[soTuyenHienTai-1] = new FragmentLinkedList<ChuyenXe>();
    	for (int i = 0; i < soTuyenHienTai - 1; i++) {      
    	    conTroVaoDanhSachTuyen[i] = temp[i];
    	}
	}
    void assignCode(string a[], string in) {
		int index = 0;
    	for (unsigned int i = 0; i < in.length(); i++) {
    	    if (in[i] == ' ') {
    	    	index++;
    	    }
    	    else {
    	   		a[index] += in[i];
    	    }
    	}
	}
	int changeToInt(string s) {
		string temp = s; 
		stringstream geek(temp);
		int x = 0;
        geek >> x;
        return x;
	}
    string query(string instruction) {
        // TODO: Your implementation
        string maLenh[6];
        assignCode(maLenh, instruction);
        if(maLenh[0] == "SQ") {
        	
			if(maLenh[1] != "") {
				soChuyenToiDaCuaTuyen = changeToInt(maLenh[1]);
        		return "1";
        	}
        	return "-1";
		}
		if(maLenh[0] == "INS") {
			ChuyenXe* temp = new ChuyenXe(maLenh[1], maLenh[2], maLenh[3], maLenh[4]);
			if(soTuyenHienTai == 0) {
				themTuyen();
				FragmentLinkedList<ChuyenXe>* chuyenList = conTroVaoDanhSachTuyen[0];
				chuyenList->add(*temp);
				return "1";						
			}
			bool khongCoTuyenNay = false;
			for (int i = 0; i < soTuyenHienTai; i++)  {
				FragmentLinkedList<ChuyenXe>::Iterator itr = conTroVaoDanhSachTuyen[i]->begin();
				ChuyenXe chuyen = (*itr); 
				if(chuyen.getMaTuyen() == maLenh[1]) {
					for(FragmentLinkedList<ChuyenXe>::Iterator it = conTroVaoDanhSachTuyen[i]->begin(); it != conTroVaoDanhSachTuyen[i]->end(); it++) {
						if((*it).bienKiemSoat == maLenh[2] && changeToInt(maLenh[3]) < changeToInt((*it).thoiGianDenCuoi)) {
							return "-1";
						}				
					}
					conTroVaoDanhSachTuyen[i]->add(*temp);
					return "1";
				}
				khongCoTuyenNay = true;
			}
			if(khongCoTuyenNay) {
				themTuyen();
				conTroVaoDanhSachTuyen[soTuyenHienTai-1]->add(*temp);
				return "1";						
			}		
			return "-1";
		}
        return "-1";
    }
};

int main()
{		
BusSystem* bs = new BusSystem();
cout << bs->query("INS 50 50D1-23342 1234 5678") << endl;
    return 0;
}

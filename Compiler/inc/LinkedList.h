#pragma once

#include <iostream>
#include <vector>
#include <cstring>

namespace jci {
	template<class T>
	class LinkedList {
	public:
		struct Link;
	private:
		Link* first;
		Link* last;
		int length;

		//Private merge function to merge two lists in sorted manner
		//Param head1 -> first link of the first linked list
		//Param head2 -> first link of the second linked list
		//Param f -> function that is used to compare link contents
		//Returns pointer to the start of the new merged linked list
		Link* merge(Link* head1, Link* head2, bool f(T, T)) {
			Link* masterHead;
			Link* tmpBack;
			if(head1 == nullptr) {
				return head2;
			}else if(head2 == nullptr) {
				return head1;
			}else{
				bool nBreak = true;
				if(f(head1->content, head2->content)) {
					masterHead = head1;
					head1 = head1->next;
					if(head1 == nullptr) {masterHead->next = head2; nBreak = false;}
				}else{
					masterHead = head2;
					head2 = head2->next;
					if(head2 == nullptr) {masterHead->next = head1; nBreak = false;}
				}
				tmpBack = masterHead;

				if(nBreak) {
					while(true) {
						if(f(head1->content, head2->content)) {
							masterHead->last = masterHead;
							masterHead->next = head1;
							masterHead = masterHead->next;
							head1 = head1->next;
							if(head1 == nullptr) {
								masterHead->next = head2;
								break;
							}
						}else{
							masterHead->last = masterHead;
							masterHead->next = head2;
							masterHead = masterHead->next;
							head2 = head2->next;
							if(head2 == nullptr) {
								masterHead->next = head1;
								break;
							}
						}
					}
				}
			}
			masterHead = tmpBack;
			return masterHead;
		}

		//Private function merge sort used to sort a list using merge sort
		//Param head -> first link of the unsorted linked list
		//Param f -> function used to compare elements from the linked list
		//Returns pointer to the first element of the new sorted list
		Link* mergeSort(Link* head, bool f(T, T)) {
			if(head == nullptr) {
				return nullptr;
			}else if(head->next == nullptr) {
				return head;
			}

			Link* slow = head;
			Link* fast = head->next;
			while(fast != nullptr) {
				fast = fast->next;
				if(fast != nullptr) {
					slow->last = slow;
					slow = slow->next;
					fast->last = fast;
					fast = fast->next;
				}
			}
			Link* ptr2 = slow->next;
			ptr2->last = slow;
			slow->next = nullptr;
			slow = nullptr;
			return merge(mergeSort(head, f), mergeSort(ptr2, f), f);
		}
		
		//Private print function used to print any given linked list
		//Param head -> first link in the target linked list
		void print(Link* head) {
			Link* current = head;
			while(current != nullptr) {
				std::cout << current->content << " ";
				current = current->next;
			}
		}
	public:
		//Internal linked list links which hold the pointers and content
		struct Link {
			T content;
			Link* next;
			Link* last;
			Link() {next = nullptr; last = nullptr;};
			//Link constructor
			//Param t -> content of the link
			//Param n -> pointer to the next link
			//Param l -> pointer to the previous link
			Link(T t, Link* n, Link* l) {content = t; next = n; last = l;};
		};

		//Constructor without parameters
		LinkedList() {
			first = nullptr;
			last = nullptr;
			length = 0;
		}

		//Constructor, create a linked list with given elements
		//Param input -> initializer list with the contents
		LinkedList(std::initializer_list<T> input) {
			first = nullptr;
			last = nullptr;
			length = 0;
			bool firstL = true;
			Link* prev = nullptr;
			for(auto item = std::rbegin(input); item != std::rend(input); ++item) {
				Link* current = new Link(*item, first, nullptr);
				if(prev != nullptr) {prev->last = current;}
				if(firstL) {last = current; firstL = false;}
				first = current;
				prev = current;
				length++;
			}
		};

		//Destructor
		~LinkedList() {
			Link* current = first;
			while(current != nullptr) {
				Link* tmp = current;
				current = current->next;
				delete tmp;
				length--;
			}
			first = nullptr;
			last = nullptr;
		};

		//Deletes the whole linked list
		void clear() {
			Link* current = first;
			while(current != nullptr) {
				Link* tmp = current;
				current = current->next;
				delete tmp;
				length--;
			}
			first = nullptr;
			last = nullptr;
		};

		//Get the length of the linked list
		//Returns the amount of elements in the linked list
		int getLength() {return length;}

		//Get the length of the target linked list
		//Param start -> first link of the target linked list
		//Returns the amount of elements in the target linked list
		int getLength(Link* start) {
			int l = 0;
			Link* current = start;
			while(current != nullptr) {
				current = current->next;
				l++;
			}
			return l;
		}

		//Get first element
		//Returns pointer to the first element
		Link* getElementFront() {return first;}

		//Get last element
		//Returns pointer to the last element
		Link* getElementBack() {return last;}

		//Iterate an iterator trough the linked list
		//Param it -> link to be iterated
		//Returns pointer to the iterated link
		Link* getElement(Link* it) {
			if(it == nullptr) {
				return first;
			}else{
				it = it->next;
			}
		}

		//Get a pointer to an element from target position from the linked list
		//Param targetPos -> position in the linked list (starts from 0, same as arrays)
		//Returns pointer to the element from target position
		Link* getElement(int targetPos) {
			int i = 0;
			Link* target;
			if(targetPos < 0) {target = nullptr;}else{target = first;}
			if(targetPos == length-1) {
				target = last;
			}else{
				if(targetPos < length/2) {
					while(i < targetPos) {
						target = target->next;
						i++;
					}
				}else{
					while(i < targetPos) {
						target = target->last;
						i++;
					}
				}	
			}
			return target;
		}

		//Swap two elements in the linked list by indexes
		//Param targetPos1 -> position of the first target element in the linked list
		//Param targetPos2 -> position of the second target element in the linked list
		void swapElements(int targetPos1, int targetPos2) {
			int i = 0;
			Link* target = getElement(targetPos1);
			Link* target2 = getElement(targetPos2);
			T temp = target->content;
			target->content = target2->content;
			target2->content = temp;
		}

		//Swap two elements in the linked list by pointers
		//param target1 -> pointer to the first target element
		//param target2 -> pointer to the second target element
		void swapElements(Link* target1, Link* target2) {
			T temp = target1->content;
			target1->content = target2->content;
			target2->content = temp;
		}

		//Reverse the elements in the linked list
		void reverseElements() {
			Link* fCurrent = first;
			Link* lCurrent = last;

			int half = length/2;
			int i = 0;
			while(i < half) {
				swapElements(fCurrent, lCurrent);
				fCurrent = fCurrent->next;
				lCurrent = lCurrent->last;
				i++;
			}
			
		}

		//Sort the elements in the linked list
		//Param f -> function that compares the elements of the linked list
		void sort(bool f(T, T)) {
			first = mergeSort(first, f);
		}

		//Connect second list to the end of the first list
		//Param A -> first linked list
		//Param B -> second linked list
		//Returns new connected linked list
		static LinkedList<T> connect(LinkedList<T>& A, LinkedList<T>& B) {
			LinkedList<T> newLL;
			int offset = 0;
			Link* AL = A.first;
			Link* BL = B.first;
			while(AL != nullptr) {newLL.addElementBack(AL->content); AL = AL->next;}
			while(BL != nullptr) {newLL.addElementBack(BL->content); BL = BL->next;}
			return newLL;
		}

		//Add a new element to the front of the linked list
		//Param element -> content of the new link
		void addElementFront(T element) {
			Link* node = new Link(element, first, nullptr);
			if(first == nullptr) {last = node;}
			first = node;
			length++;
		}

		//Add a new element to the front of the linked list
		//Param element -> content of the new link
		void addElementBack(T element) {
			Link* node = new Link(element, nullptr, last);
			if(last != nullptr) {last->next = node; node->last = last;}
			if(first == nullptr) {first = node;}
			last = node;
			length++;
		}

		//Add a new element after the target position to the linked list
		//Param element -> content of the new link
		//Param targetPos -> target position of the new link
		void addElement(T element, int targetPos) {
			Link* prev = getElement(targetPos-1);
			Link* target = prev->next;
			Link* current = new Link(element, target, prev);
			if(prev != nullptr) {prev->next = current;}
			if(targetPos == 0) {first = current;}
			if(targetPos == length-1) {last = current;}
			length++;
		}

		//Remove last element from the linked list
		void removeElementBack() {
			Link* temp = last->last;
			last = last->last; last->next = nullptr;
			delete temp;
		}

		//Remove the first element from the list
		void removeElementFront() {
			Link* temp = first->next;
			delete first;
			first = temp;
		}

		//Remove an element from the target position from the linked list
		//Param targetPos -> position of the target element
		void removeElement(int targetPos) {
			Link* prev = getElement(targetPos-1);
			Link* target = prev->next;
			if(targetPos == 0) {
				Link* temp = first->next;
				delete first;
				first = temp;
			}else{
				prev->next = target->next;
				delete target;
			}
			if(targetPos == length-1) {
				Link* temp = prev;
				last = prev; prev->next = nullptr;
				delete temp;
			}
			length--;
		}

		//Remove an element from the linked list
		//Param target -> pointer to the target element
		void removeElement(Link* target) {
			Link* prev = target->last;
			if(prev == nullptr) {
				Link* temp = first->next;
				delete first;
				first = temp;
			}else{
				prev->next = target->next;
				delete target;
			}
			if(prev == last) {
				Link* temp = prev;
				last = prev; prev->next = nullptr;
				delete temp;
			}
			length--;
		}

		//Apply a function to all elements of the linked list
		//Param f -> function to be applied to all elements of the linked list
		void applyFunction(T f(T)) {
			Link* current = first;
			while(current != nullptr) {
				current->content = f(current->content);
				current = current->next;
			}
		}

		//Apply a function to all elements of the linked list and remove ones that return false
		//Param f -> function to be applied to all elements 
		void filterFunction(bool f(T)) {
			Link* current = first;
			while(current != nullptr) {
				bool result = f(current->content);
				if(!result) {removeElement(current);}
				current = current->next;		
			}
		}

		//Apply a function to pairs of elements from two linked lists
		//Param A -> first linked list
		//Param B -> second linked list
		//Returns new linked list with processed elements
		static LinkedList<T> pairs(LinkedList<T>& A, LinkedList<T>& B, T f(T, T)) {
			LinkedList<T> newLL;
			Link* currentA = A.first;
			Link* currentB = B.first;
			while(currentA != nullptr && currentB != nullptr) { 
				newLL.addElementFront(f(currentA->content, currentB->content));
				currentA = currentA->next;
				currentB = currentB->next;
			}
			return newLL;
		}

		//Remove duplicates from sequences in the linked list
		//Param f -> function to compare elements with
		void unique_seq(bool f(T, T)) {
			Link* prev = nullptr;
			Link* current = first;
			while(current != nullptr) {
				Link* temp = current->next;
				if(temp != nullptr) {
					while(f(temp->content, current->content)) {
						removeElement(temp);
					}
					temp = temp->next;
				}
				prev = current;
				current = current->next;
			}
		}

		//Remove duplicates from the linked list
		//Param f -> function to compare elements with
		void unique(bool f(T, T)) {
			std::vector<T> seen;
			Link* current = first;
			while(current != nullptr) {
				bool exists = false;
				for(auto v : seen) {
					if(f(current->content, v)) {exists = true; break;}
				}
				if(exists) {
					removeElement(current);
				}else{
					seen.push_back(current->content);
				}
				current = current->next;
			}
		}

		//Find if element exists in the linked list
		//Param x -> target content
		//Param f -> function to compare that content with elements of the linked list
		//Returns true if given content exists in the linked list, false otherwise
		bool exists(T x, bool f(T, T)) {
			bool exists = false;
			Link* current = first;
			while(current != nullptr) {
				if(f(x, current->content)) {exists = true; break;}
				current = current->next;
			}
			return exists;
		}

		//Print all of the elements of the linked list
		void print() {
			Link* current = first;
			while(current != nullptr) {
				std::cout << current->content << " ";
				current = current->next;
			}
			std::cout << '\n';
		};
	};	
}


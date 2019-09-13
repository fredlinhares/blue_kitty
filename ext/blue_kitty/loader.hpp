// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_LOADER_H
#define BLUE_KITTY_LOADER_H 1

#include <string>
#include <vector>

namespace Loader
{
class Error
{
 public:
  Error();
  Error(const std::string &m);
  Error(const char &m);
  std::string message;
};

template<class T>
struct Actions
{
  void (T::*load)(void);
  void (T::*unload)(void);
};

template<class T>
class Stack
{
 public:
  Stack(T *object);
  ~Stack();

  void add(void (T::*load)(void), void (T::*unload)(void));
  void load();
  bool reload(int step);
  void unload();

 private:
  T *obj;
  bool loaded;
  int last_loaded;
  std::vector<Actions<T>> actions;

  void step_load(int step);
  void step_unload(int step);
};

template<class T>
Stack<T>::Stack(T *object):
    obj{object},
    loaded{false},
    last_loaded{0}
{
}

template<class T>
Stack<T>::~Stack()
{
  if(loaded) this->unload();
}

template<class T>
void Stack<T>::add(void (T::*load)(void), void (T::*unload)(void))
{
  Actions<T> a;
  a.load = load;
  a.unload = unload;
  actions.push_back(a);
}

template<class T>
void Stack<T>::load()
{
  return step_load(actions.size());
}

template<class T>
void Stack<T>::unload()
{
  step_unload(0);
}

template<class T>
bool Stack<T>::reload(int step)
{
  if(!loaded) return false;
  this->step_unload(step);
  this->step_load(actions.size());
  return true;
}

template<class T>
void Stack<T>::step_load(int step)
{
  Error error_message;
  bool error = false;

  if(this->loaded) return;
  loaded = true;

  for(; this->last_loaded < step; this->last_loaded++)
  {
    try
    {
      (this->obj->*(this->actions[this->last_loaded].load))();
    }
    catch(Error le)
    {
      error = true;
      error_message = le;
      break; // Stop if load falied.
    }
  }

  // This number will be one after the last loeaded after the loading loop is
  // over.
  this->last_loaded--;

  if(error)
  {
    // Backward interation to unload what was already loaded.
    this->step_unload(0);
    throw error_message;
  }
}

template<class T>
void Stack<T>::step_unload(int step)
{
  if(!this->loaded) return;
  this->loaded = false;

  for(; this->last_loaded > step - 1; this->last_loaded--)
    (this->obj->*(this->actions[this->last_loaded].unload))();

  // This number will be one before the last loeaded after the unloading loop
  // is over.
  this->last_loaded++;
}
}
#endif // BLUE_KITTY_LOADER_H

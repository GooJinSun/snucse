#include <iostream>
#include <fstream>
#include <vector>
#include <forward_list>
#include <string>
#include <algorithm>
#include <cassert>

using std::string;
using std::vector;
using std::ostream;


//
// 에러메세지를 출력하고 프로그램 종료
//
[[ noreturn ]]
void panic(const char *msg) {
  using namespace std;

  cerr << "\n\n" << msg << "\n\n";
  exit(1);
}


class Item {
public:
  enum class Type { normal, flammable, ice };

  static Type type_from_ch(char input) {
    switch (input) {
      case 'N': return Type::normal;
      case 'F': return Type::flammable;
      case 'I': return Type::ice;
      default: panic("Invalid item type character has been given");
    }
  }

  const string name;
  const Type type;
  const size_t size;

  Item(const string &name, char type, size_t size) :
    name { name }, type { type_from_ch(type) }, size { size } {}
  Item(string &&name, char type, size_t size) :
    name { name }, type { type_from_ch(type) }, size { size } {}
};


class Container {
public:
  enum class Type { normal, protect, cold };

  // char 로부터 Type을 얻어내는 함수
  static Type type_from_ch(char input) {
    switch (input) {
      case 'N': return Type::normal;
      case 'P': return Type::protect;
      case 'C': return Type::cold;
      default: panic("Invalid container type character has been given");
    }
  }

  const Type type;
  const size_t size;
  const size_t cost;

private:
  vector<Item> items;

public:
  Container(char type, size_t size, size_t cost) :
    type { type_from_ch(type) }, size { size }, cost { cost } {}

  void put(const Item &item) {
    items.push_back(item);
  }

  size_t remain() const {
    size_t remain = size;
    for (const auto& item: items) {
      remain -= item.size;
    }
    return remain;
  }
};


ostream& operator<<(ostream& os, Item::Type ty) {
  switch (ty) {
    case Item::Type::normal:    return os << "normal";
    case Item::Type::flammable: return os << "flammable";
    case Item::Type::ice:       return os << "ice";
  }
}

ostream& operator<<(ostream& os, Container::Type ty) {
  switch (ty) {
    case Container::Type::normal:  return os << "normal";
    case Container::Type::protect: return os << "protect";
    case Container::Type::cold:    return os << "cold";
  }
}


int main(int argc, char* argv[]) {
  using namespace std;

  // ARGV 파싱
  if (argc != 3) {
    cerr << "Wrong usage." << endl;
    cerr << endl;
    cerr << "Example:" << endl;
    cerr << "    " << argv[0] << " input.txt output.txt" << endl;
    return 1;
  }
  assert(argc == 3);

  const auto iname = argv[1];
  const auto oname = argv[2];

  // 파일 입력 받아서 자료구조에 저장
  vector<Container> containers;
  forward_list<Item> items;
  {
    auto input = ifstream { iname };
    size_t item_count;
    input >> item_count;

    for (size_t _ = 0; _ < 3; ++_) {
      char type;
      size_t size, cost;
      input >> size >> type >> cost;
      containers.emplace_back(type, size, cost);
    }

    for (size_t _ = 0; _ < item_count; ++_) {
      string name;
      char type;
      size_t size;
      input >> name >> size >> type;
      items.push_front(Item { move(name), type, size });
    }
  }

  // 세 컨테이너 각각 찾아서 변수에 저장
  auto find = [&](Container::Type ty) -> Container& {
    auto it = find_if(containers.begin(), containers.end(), [=](const Container& c) {
      return c.type == ty;
    });
    if (it == containers.end()) { panic("Wrong Input : 특정 타입의 컨테이너가 주어지지 않았음"); }
    return *it;
  };

  auto &normal  = find(Container::Type::normal);
  auto &protect = find(Container::Type::protect);
  auto &cold    = find(Container::Type::cold);

  // 타는물건이랑 얼음은 행선지가 유일하므로 일단 해당 컨테이너에 넣고 봄
  for (const auto& item: items) {
    switch (item.type) {
    case Item::Type::normal: break;
    case Item::Type::flammable: protect.put(item); break;
    case Item::Type::ice: cold.put(item); break;
    }
  }
  items.remove_if([](const Item &item) {
    return item.type != Item::Type::normal;
  });

  // TODO: 남아있는 컨테이너로 냅색을 해야됨
  for (const auto& container: containers) {
    cout << container.type << " = { size: " << container.remain() << '/' << container.size << ", cost: " << container.cost << " }\n";
  }
  for (const auto& item: items) {
    cout << item.name << ": " << item.type << " = " << item.size << endl;
  }

  return 0;
}

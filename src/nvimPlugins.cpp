#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>
// NOTE: getmax gives the y and x that out of window ( border ) so always use y-1 and x-1

const unsigned int XOF = 13;

enum keys
{
  UP = 'k',
  DOWN = 'j',
  QUIT = 'q',
  ENTER = '\n',
  DELETE = 127,
  SEARCH = '/',
  ESC = '\033'
};

std::vector<std::string> filterEnt(const std::vector<std::string> &sourece, const std::string_view &input)
{
  std::vector<std::string> tmp;
  tmp.reserve(sourece.size() - 7);
  for (const std::string &ent : sourece)
  {
    if (ent.find(input) != std::string::npos)
    {
      tmp.push_back(ent);
    }
  }

  return tmp;
}

void printMenu(const std::vector<std::string> &entries, const unsigned int highlighter, WINDOW *win, unsigned int yMax)
{
  unsigned int i = 0, y = 0;
  unsigned int xOffset = 1;
  unsigned int eCout = entries.size();
  for (; i < eCout; i++, y++)
  {
    if (y > yMax - 2)
    {
      xOffset += XOF;
      y = 0;
    }
    if (highlighter == i)
    {
      attron(A_REVERSE);
      wmove(win, y + 1, xOffset);
      wprintw(win, "%s", entries[i].c_str());
      attroff(A_REVERSE);
    }
    else
    {
      wmove(win, y + 1, xOffset);
      wprintw(win, "%s", entries[i].c_str());
    }
  }
}

char picker(WINDOW *win, const std::vector<std::string> &entries, const int &yMax, unsigned int &highlighter)
{
  printMenu(entries, highlighter, win, yMax);
  while (1)
  {
    char key = wgetch(win);
    switch (key)
    {
    case UP:
      if (highlighter <= 0)
      {
        highlighter = entries.size() - 1;
        printMenu(entries, highlighter, win, yMax);
      }
      else
      {
        printMenu(entries, --highlighter, win, yMax);
      }
      break;
    case DOWN:
      if (highlighter >= entries.size() - 1)
      {
        highlighter = 0;
        printMenu(entries, highlighter, win, yMax);
      }
      else
      {
        printMenu(entries, ++highlighter, win, yMax);
      }
      break;
    default:
      return key;
      break;
    }
  }
}

char searchMode(const std::vector<std::string> &entries, WINDOW *win, const unsigned int &yMax, std::string &option)
{
  wclear(win);
  std::vector<std::string> filterd = entries;
  std::string searchInput;
  bool end = 0;
  bool picking = 0;
  while (true)
  {
    if (picking)
    {
      if (filterd.size() == 1)
      {
        option = filterd[0];
        return 's';
      }
      unsigned int highlighter = 0;
      while (picking)
      {
        switch (picker(win, filterd, yMax, highlighter))
        {
        case ESC:
          return 'e';
        case ENTER:
          option = filterd[highlighter];
          return 's';
        case SEARCH:
          picking = 0;
          searchInput.clear();
          break;
        case QUIT:
          return 'q';
        default:
          break;
        }
      }
      continue;
    }
    wclear(win);
    printMenu(filterd, 0, win, yMax - 3);
    mvwprintw(win, yMax - 1, 0, "/%s", searchInput.c_str());
    while (!picking) // opt
    {
      char key = wgetch(win);
      switch (key)
      {
      case ENTER:
        picking = 1;
        break;
      case DELETE:
        if (!searchInput.empty())
        {
          searchInput.pop_back();
          filterd = filterEnt(entries, searchInput);
          wclear(win);
          printMenu(filterd, 0, win, yMax - 3);
          mvwprintw(win, yMax - 1, 0, "/%s", searchInput.c_str());
        }
        break;
      case ESC:
        return 'e';
        break;
      default:
        if (key != ' ')
        {
          searchInput.push_back(key);
          filterd = filterEnt(entries, searchInput);
          wclear(win);
          printMenu(filterd, 0, win, yMax - 3);
          mvwprintw(win, yMax - 1, 0, "/%s", searchInput.c_str());
        }
      }
    }
  }
}

std::string Menu(const std::vector<std::string> &entries, WINDOW *win)
{

  unsigned int highlighter = 0;
  unsigned int lastEnt = entries.size() - 1;
  unsigned int yMax = getmaxy(win);
  char key;
  while (1)
  {
    switch (picker(win, entries, yMax, highlighter))
    {
    case QUIT:
      return "q";
    case ENTER:
      return entries[highlighter];
    case SEARCH:
    {
      std::string option;
      switch (searchMode(entries, win, yMax, option))
      {
      case 'e':
        wclear(win);
        break;
      case 'q':
        return "q";
      case 's':
        return option;
        ;
      }
    }
    default:
      break;
    }
  }
}

int main()
{
  std::string home = getenv("HOME");

  using namespace std::filesystem;
  std::filesystem::path path = home + "/.config/nvim/lua/plugins";
  if (!exists(path))
    std::cout << "Dir doesn't exist: " + path.string();
  if (!is_directory(path))
    std::cout << "Path isn't a dir: " + path.string();

  std::vector<std::string> entries;
  entries.reserve(25);
  for (const directory_entry &ent : directory_iterator(path))
  {
    entries.push_back(ent.path().stem().string());
  }

  // Starting ncurses
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  std::string choice = Menu(entries, stdscr);
  endwin();

  if (choice.c_str()[0] == 'q')
  {
    return 0;
  }
  std::cout << choice;
}

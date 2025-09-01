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
  DELETE = 8,
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

void searchMode(const std::vector<std::string> &entries, WINDOW *win, unsigned int yMax)
{
  wclear(win);
  std::vector<std::string> filterd = entries;
  std::string searchInput;
  bool end = 0;
  while (!end)
  {
    wclear(win);
    printMenu(filterd, 0, win, yMax - 3);
    mvwprintw(win, yMax - 1, 0, "/%s", searchInput.c_str());
    char key = wgetch(win);
    switch (key)
    {
    case ENTER:
      break;
    case DELETE:
      searchInput.pop_back();
      filterd = filterEnt(entries, searchInput);
      break;
    case ESC:
      end = 1;
      break;
    default:
      searchInput.push_back(key);
      filterd = filterEnt(entries, searchInput);
    }
  }
}

keys getAllowedKey(WINDOW *win)
{
  while (true)

  {

    switch (wgetch(win))
    {
    case UP:
      return UP;

    case DOWN:
      return DOWN;

    case QUIT:
      return QUIT;

    case ENTER:
      return ENTER;
    case SEARCH:
      return SEARCH;
    default:
      break;
    }
  }
}

std::string Menu(const std::vector<std::string> &entries, WINDOW *win)
{

  unsigned int highlighter = 0;
  unsigned int lastEnt = entries.size() - 1;
  unsigned int yMax = getmaxy(win);

  while (1)
  {
    printMenu(entries, highlighter, win, yMax);
    switch (getAllowedKey(win))
    {
    case UP:
      if (highlighter > 0)
        highlighter--;
      break;
    case DOWN:
      if (highlighter < lastEnt)
        highlighter++;
      break;
    case QUIT:
      return "q";
      break;
    case ENTER:
      return entries[highlighter];
      break;
    case SEARCH:
      searchMode(entries, win, yMax);
      break;
    default:
      break;
      // I made getAllowedKey fun to prevent printMenu when invalid key is enterd
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
  noecho();
  curs_set(0);
  std::string choice = Menu(entries, stdscr);
  // searchMode(entries, stdscr, getmaxy(stdscr));

  endwin();

  // std::cout << choice;
}

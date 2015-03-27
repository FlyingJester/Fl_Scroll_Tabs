//
// "$Id: Fl_Scroll_Tabs.H $"
//
// Scrolling tabs for dynamic content header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//



/* \file
   Fl_Tabs widget . */

#ifndef Fl_Scroll_Tabs_H
#define Fl_Scroll_Tabs_H

#include <FL/Fl_Tabs.H>

/**
  The Fl_Scroll_Tabs class implements a scrolling, dynamic tabs widget for FLTK.

  This widget is intended to hold dynamic content, and allows the user to close
  tabs using a close button beside tab's label.
  If too many tabs exist to all have their labels displayed at once, the labels
  can be scrolled through using a pair of buttons.
*/
class FL_EXPORT Fl_Scroll_Tabs : public Fl_Tabs {

  unsigned long offset;   
  Fl_Widget *value_;
  int closebutton_;
  Fl_Callback_p close_callback_;
  void *close_callback_arg_;
  
  unsigned tab_height_, button_width_;
  int minimum_tab_width_, maximum_tab_width_;

  int pressed_; // 0 for tab bar, 1 for left button, 2 for right button
  int ensure_value();
  int calculate_tab_sizes();

  int *tab_pos;  // array of x-offsets of tabs per child
  int *tab_width;  // array of widths of tabs per child
  char **tab_labels; 
  int tab_count;  // size for tab_pos and tab_width

  int tab_positions();  // allocate and calculate tab positions
  int tab_label_length(int);  // calculate the label to print for the tab `i'. Returns the width of the new label (total) in pixels.
  void clear_tab_positions();

protected:
    
    // Callbacks for scrolling the list of tabs
  virtual int can_scroll_left() const;
  virtual int can_scroll_right() const;
  virtual void increment_cb();
  virtual void decrement_cb();
    
  void do_scroll_cb() {if (pressed_==1) increment_cb(); else if (pressed_==2) decrement_cb();}

  static void timeout_cb(void *);

  virtual void draw();
    
public:
    
  Fl_Scroll_Tabs(int,int,int,int,const char *l=0);
  ~Fl_Scroll_Tabs();
  
  virtual int handle(int);

  Fl_Widget *push() const;
  int push(Fl_Widget *);
  int push(int i) {return push(child(i));}
  
  /**
    Returns the currently selected tab group, or NULL if no tabs exist.
    \see value(Fl_Widget*).
  */
  Fl_Widget *value() {ensure_value(); return value_;}
  
  /**
    Sets the currently selected tab to \p w if it is a child. Returns the child's index in array()
    if it is a child, children() otherwise.
    \see array()
  */
  int value(Fl_Widget *w){int f = find(w); if (f!=children()) value_ = w; return f;}
  
  /**
    Returns the tab group that would be selected by clicking at \p event_x, \p event_y.
    Returns NULL if the location would not select a tab group.
  */
  Fl_Widget *which(int event_x, int event_y);
  
  /**
    Returns non-zero if the close button is displayed on tabs, zero otherwise.
    \see set_closebutton()
    \see clear_closebutton()
    \see closebutton(int)
  */
  int closebutton() const {return closebutton_!=0;}
  
  /**
    Sets whether or not to display the close button on tab labels
    \see set_closebutton()
    \see clear_closebutton()
    \see closebutton()
  */
  void closebutton(int i) {closebutton_=(i!=0);tab_positions();}
  
  /**
    Sets the close button to be displayed on tabs  
    \see clear_closebutton()
    \see closebutton(int)
    \see closebutton()
  */
  void set_closebutton() {closebutton(1);tab_positions();} 
  /**
    Sets the close button not to be displayed on tabs  
    \see set_closebutton()
    \see closebutton(int)
    \see closebutton()
  */
  void clear_closebutton() {closebutton(0);tab_positions();}
  
  /**
    Sets a callback to for when a tab group is closed using a close button next to its label. 
    This callback will be called after removing the widget from the tabs, so it is alright to delete the widget in the callback.
    \see closebutton()
  */
  void close_callback(Fl_Callback_p cb_, void *arg_) {close_callback_ = cb_; close_callback_arg_ = arg_;}
  
  /**
    Sets the maximum and minimum tab label sizes. This limits the sizes, which are determined by the size of the label text.
    Setting a maximum of -1 allows any size tab.
    \see tab_size_range(int &, int &)
  */
  void tab_size_range(int maxw, int minw){
    minimum_tab_width_ = minw;
    maximum_tab_width_ = maxw;
    tab_positions();
  }
  
  /**
    Gets the maximum and minimum tab label sizes. A maximum of -1 indicates no upper limit to tab size.
    \see tab_size_range(int, int)
  */
  void tab_size_range(int &maxw, int &minw) const {
    minw = minimum_tab_width_;
    maxw = maximum_tab_width_;
  }
  
};

#endif

#pragma once
#include <FL/Fl_Group.H>

class Fl_Scroll_Tabs : public Fl_Group {
protected:

    unsigned long offset;   
    Fl_Widget *value_;
    int closebutton_;
    Fl_Callback_p close_callback_;
    void *close_callback_arg_;
        
    unsigned tab_height_, button_width_;
    
    int pressed_; // 0 for tab bar, 1 for left button, 2 for right button
    int ensure_value();
    int calculate_tab_sizes();
    
    // Callbacks for scrolling the list of tabs
    virtual int can_scroll_left() const;
    virtual int can_scroll_right() const;
    virtual void increment_cb();
    virtual void decrement_cb();
    
    void do_scroll_cb(){if(pressed_==1) increment_cb(); else if(pressed_==2) decrement_cb();}
    
    static void timeout_cb(void *);

    virtual void draw();
    
public:
    
    Fl_Scroll_Tabs(int,int,int,int,const char *l=0);
    
    virtual int handle(int);

    Fl_Widget *push() const;
    int push(Fl_Widget *);
    int push(int i) {return push(child(i));}
    Fl_Widget *value() {ensure_value(); return value_;}
    int value(Fl_Widget *w){int f = find(w); if(f!=children()) value_ = w; return f;}
    
    int closebutton() const{return closebutton_!=0;}
    void closebutton(uchar i){closebutton_=(i!=0);}
    
    void set_closebutton(){closebutton(1);}
    void clear_closebutton(){closebutton(0);}
   
    void close_callback(Fl_Callback_p cb_, void *arg_) {close_callback_ = cb_; close_callback_arg_ = arg_;}

};

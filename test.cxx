#include "Fl_Scroll_Tabs.H"
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/fl_ask.H>
#include <FL/Fl.H>

//#define TABS Fl_Scroll_Tabs
#define TABS Fl_Tabs

//#define GPOS 24
#define GPOS 0

Fl_Color initial_selection_color;

void button_cb(Fl_Widget *w, void *a){
    fl_alert("Button pressed.\n");
}

void change_selection_color_cb(Fl_Widget *w, void *a){
    Fl_Button *b = static_cast<Fl_Button*>(w);
    Fl_Scroll_Tabs *that = static_cast<Fl_Scroll_Tabs*>(a);

    if(b->value())
        that->selection_color(FL_RED);
    else
        that->selection_color(initial_selection_color);

    that->redraw();
    
}

int main(int arc, char *argv[]){
    
    Fl_Window window(400, 200, "Fl_Scroll_Tabs Test");
  
    TABS *scroll_tabs = new TABS(0, 0, 400, 200);

    initial_selection_color = scroll_tabs->selection_color();
    //scroll_tabs->set_closebutton();
    
    Fl_Group *group1 = new Fl_Group(0, GPOS, 400, 200-24, "Test Tab 1");
    group1->color(FL_BLUE);
    group1->box(FL_FLAT_BOX);
    group1->end();

    Fl_Group *group2 = new Fl_Group(0, GPOS, 400, 200-24, "Interaction Test");
    Fl_Button *button = new Fl_Button(24, 48+GPOS, 128, 24, "Press");
    button->callback(button_cb, NULL);
    group2->end();

    Fl_Group *group3 = new Fl_Group(0, GPOS, 400, 200-24, "Color Change Tab");
    Fl_Button *color_button = new Fl_Light_Button(24, 48+GPOS, 128, 24, "Selection Color");
    color_button->callback(change_selection_color_cb, scroll_tabs);
    group3->end();

    Fl_Group *group4 = new Fl_Group(0, GPOS, 400, 200-24, "Filler Tab 4");
    group4->end();

    Fl_Group *group5 = new Fl_Group(0, GPOS, 400, 200-24, "Filler Tab 5");
    group5->end();
    scroll_tabs->end();
    // scroll_tabs->resizable(group1);
 
    window.end();
    window.resizable(window);

    window.show();

    return Fl::run();
}

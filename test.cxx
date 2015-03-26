#include "Fl_Scroll_Tabs.H"
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>
#include <FL/Fl.H>

void button_cb(Fl_Widget *w, void *a){
    fl_alert("Button pressed.\n");
}

int main(int arc, char *argv[]){
    
    Fl_Window window(400, 200, "Fl_Scroll_Tabs Test");

    Fl_Scroll_Tabs *scroll_tabs = new Fl_Scroll_Tabs(0, 0, 400, 200);
    Fl_Group *group1 = new Fl_Group(0, 24, 400, 200-24, "Test Tab 1");
    group1->color(FL_BLUE);
    group1->box(FL_FLAT_BOX);
    scroll_tabs->begin();

    Fl_Group *group2 = new Fl_Group(0, 24, 200, 400-24, "Interaction Test");
    Fl_Button *button = new Fl_Button(24, 48, 64, 24, "Press");
    button->callback(button_cb, NULL);

    window.show();

    return Fl::run();
}

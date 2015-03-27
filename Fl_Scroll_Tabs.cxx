#include "Fl_Scroll_Tabs.H"
#include <FL/fl_draw.H>
#include <FL/Fl.H>

#define LABEL_WIDTH 128
#define TAB_SCROLL 8
#define MINIMUM_TAB_HEIGHT 16

// The amount to pad tabs that are not selected
#define TAB_SELECTION_BORDER 2

#define INITIALREPEAT .5
#define REPEAT .01

Fl_Scroll_Tabs::Fl_Scroll_Tabs(int ax, int ay, int aw, int ah, const char *l)
  : Fl_Tabs(ax, ay, aw, ah, l)
  , offset(0)
  , value_(NULL)
  , closebutton_(1)
  , close_callback_(NULL)
  , tab_height_(MINIMUM_TAB_HEIGHT)
  , button_width_(MINIMUM_TAB_HEIGHT)
  , pressed_(-1) {
  box(FL_FLAT_BOX);
}

Fl_Scroll_Tabs::~Fl_Scroll_Tabs() {
  if(pressed_)
    Fl::remove_timeout(timeout_cb, this);
}

Fl_Widget *Fl_Scroll_Tabs::push() const {
  return (pressed_==0)?value_:NULL;
}

int Fl_Scroll_Tabs::push(Fl_Widget *w){
  Fl_Widget * const old_value = value_;
  value_ = w;
  const int c = ensure_value(); 
    
  if (value_ && (value_!=old_value)){
  if (when()&FL_WHEN_CHANGED)
    do_callback();
    redraw();
  }
  else {
    draw_child(*value_);
    value_->set_visible_focus();
  }
  
  return c;
}

int Fl_Scroll_Tabs::handle(int e) {

  ensure_value();

  switch (e) {
    case FL_PUSH:
    /* FALLTHROUGH */
    case FL_DRAG:
    /* FALLTHROUGH */
    case FL_RELEASE:
    /* FALLTHROUGH */
    case FL_MOVE:
    /* FALLTHROUGH */
    case FL_MOUSEWHEEL:
    if (Fl::event_y()<y()+tab_height_) { // Is withing tab bar
      const int inside_left_button = Fl::event_x()<=x()+button_width_,
        inside_right_button = Fl::event_x()>=x()+w()-button_width_;
      if (e==FL_PUSH) {
        if (inside_left_button) {
          increment_cb();
          pressed_ = 1;
        }
        else if (inside_right_button) {
          decrement_cb();
          pressed_ = 2;
        }
        else{
          pressed_ = 0;
        }

        if (inside_left_button || inside_right_button) {
          Fl::add_timeout(INITIALREPEAT, timeout_cb, this);
            return 1;
        }
      }
      if (e==FL_RELEASE) {
        
        if (pressed_!=-1)
          // We need to redraw the scroll button
          redraw();

        if(pressed_>0)
          Fl::remove_timeout(timeout_cb, this);

        pressed_ = -1;

        if (!(inside_left_button || inside_right_button)) { // Mouse is inside the tab bar itself
          Fl_Widget *const kid = which(Fl::event_x(), Fl::event_y());

          if (!kid)
            return 1;
          else if (closebutton_ && ((Fl::event_x()+offset-button_width_)%LABEL_WIDTH > LABEL_WIDTH-button_width_)) {
            remove(kid);
            if (close_callback_)
              close_callback_(kid, close_callback_arg_);
            ensure_value();
            redraw();
          }
          else
            push(kid);
                      
          return 1;
        }
      }
    } // Is withing tab bar
    else if ((e==FL_PUSH) || (e==FL_RELEASE)) {
      if(pressed_)
        redraw();
      pressed_=-1;
    }
  }

  return Fl_Group::handle(e);
}

void Fl_Scroll_Tabs::draw() {
  
  // Draw our box
  fl_draw_box(box(), x(), y(), w(), h(), color());

  if(!children())
    return;

  const int X = x()+Fl::box_dx(box()),
    Y = y()+Fl::box_dy(box()),
    W = w()-Fl::box_dw(box()),
    H = h()-Fl::box_dh(box());

  calculate_tab_sizes();
  
  Fl_Boxtype l_button_box = (pressed_==1)?FL_DOWN_FRAME:FL_UP_FRAME;
  Fl_Boxtype r_button_box = (pressed_==2)?FL_DOWN_FRAME:FL_UP_FRAME;
  
  // Draw the left and right buttons.
  fl_draw_box(l_button_box, X, Y, button_width_, tab_height_, color());
  fl_draw_box(r_button_box, X+W-button_width_, Y, button_width_, tab_height_, color());
    
  {
    // Draw the arrows    
    const int l_button_x = X+Fl::box_dx(l_button_box),
      l_button_y = Y+Fl::box_dy(l_button_box),
      l_button_w = button_width_-Fl::box_dw(l_button_box),
      l_button_h = tab_height_-Fl::box_dh(l_button_box);
            
    const int r_button_x = X+W+Fl::box_dx(r_button_box)-button_width_,
      r_button_y = Y+Fl::box_dy(r_button_box),
      r_button_w = button_width_-Fl::box_dw(r_button_box),
      r_button_h = tab_height_-Fl::box_dh(r_button_box);
    
    fl_color(can_scroll_left()?labelcolor():fl_inactive(labelcolor()));
    fl_polygon(l_button_x+((l_button_w<<1)/3), l_button_y+(r_button_h/4),
      l_button_x+(l_button_w/3), l_button_y+(l_button_h>>1),
      l_button_x+((l_button_w<<1)/3), l_button_y+((l_button_h*3)/4));
   
    fl_color(can_scroll_right()?labelcolor():fl_inactive(labelcolor()));
    fl_polygon(r_button_x+(r_button_w/3), r_button_y+(r_button_h/4),
      r_button_x+((r_button_w<<1)/3), r_button_y+(r_button_h>>1),
      r_button_x+(r_button_w/3), r_button_y+((r_button_h*3)/4));
  }

  {
    fl_font(labelfont(), labelsize());
    
    // Clip to the tab bar
    fl_push_clip(x()+button_width_, y(), w()-(button_width_<<1), tab_height_);
        
    const int font_offset = (tab_height_+fl_height())>>1;
        
    // Draw children.
    for (int i = 0; i<children(); i++) {
      // The x of the current tab we want to draw.
      const int that_x = X+(i*LABEL_WIDTH)-offset+button_width_;
            
      if (fl_not_clipped(that_x, Y, LABEL_WIDTH, tab_height_)==0)
        continue;

      // Draw the frame for the tab panel
      if (child(i)==value_)
        fl_draw_box(FL_DOWN_BOX, that_x-2, Y, LABEL_WIDTH, tab_height_+2+TAB_SELECTION_BORDER, selection_color());
      else
        fl_draw_box(FL_UP_BOX, that_x, Y+2, LABEL_WIDTH-4, tab_height_+2, color());
                        
      // Draw the tab title
      fl_push_clip(that_x, Y, LABEL_WIDTH-tab_height_, tab_height_);
      fl_color(labelcolor());
      fl_draw(child(i)->label(), that_x, Y+font_offset);
      fl_pop_clip();

      if (closebutton_) {
        // Draw the close button
        fl_draw_box(FL_THIN_DOWN_FRAME, that_x+LABEL_WIDTH-button_width_-1, Y+3, button_width_-4, tab_height_-4, color());
        fl_color(labelcolor());
        // Draw a closed loop as so:
        /*   v-v <= cross_edge_diff
                  <
                  | <= cross_insets
                  <
                2
               / \
              1   \
               .   \
                .   \
                 .   3
                  . /
                   4
      ...
                   1
                  . \
                 .   2
                .   /
               .   /
              4   /
               \ /
                3  
        */
        
        const int box_bound_x = that_x+LABEL_WIDTH-button_width_-2+Fl::box_dx(FL_THIN_DOWN_FRAME),
          box_bound_y = Y+4,
          box_bound_w = button_width_-4-Fl::box_dw(FL_THIN_DOWN_FRAME),
          box_bound_h = tab_height_-4-Fl::box_dh(FL_THIN_DOWN_FRAME),
          cross_insets = 2, cross_edge_diff = 1;

        // Top left to bottom right
        fl_polygon(box_bound_x+cross_insets, box_bound_y+cross_insets+cross_edge_diff,
          box_bound_x+cross_insets+cross_edge_diff, box_bound_y+cross_insets,
          box_bound_x+box_bound_w-cross_insets, box_bound_y+box_bound_h-cross_insets-cross_edge_diff,
          box_bound_x+box_bound_w-cross_insets-cross_edge_diff, box_bound_y+box_bound_h-cross_insets);
        
        // Top right to bottom left
        fl_polygon(box_bound_x+box_bound_w-cross_insets-cross_edge_diff, box_bound_y+cross_insets,
          box_bound_x+box_bound_w-cross_insets, box_bound_y+cross_insets+cross_edge_diff,
          box_bound_x+cross_insets+cross_edge_diff, box_bound_y+box_bound_h-cross_insets,
          box_bound_x+cross_insets, box_bound_y+box_bound_h-cross_insets-cross_edge_diff);
        
        
        //fl_polygon();
      }
    }

    fl_pop_clip();
  }    
    // Draw the selected child.
    fl_push_clip(x(), y()+16, w(), h()-16);
    
    if (value_)
        draw_child(*value_);
    
    fl_pop_clip();

}

/**
  Return the widget of the tab the user clicked on at \p event_x / \p event_y.
  This is used for event handling (clicks) and by fluid to pick tabs.

  \returns The child widget of the tab the user clicked on, or<br>
           0 if there are no children or if the event is outside of the tabs area.
*/
Fl_Widget *Fl_Scroll_Tabs::which(int event_x, int event_y) {
  if ((event_y>y()+tab_height_) || (event_y<y()) || 
    (event_x<x()+button_width_) || (event_x>x()+w()-button_width_))
    return NULL;
  
  const int selected_tab = (event_x+offset-button_width_)/LABEL_WIDTH;

  if ((selected_tab>=children()) || (selected_tab<0)) return NULL;
    
  return child(selected_tab);
}

int Fl_Scroll_Tabs::calculate_tab_sizes() {
  if (!children()) return 1;
    
  int lowest_top = h();
  for (int i = 0; i<children(); i++) {
    const int top_diff = child(i)->y()-y();
    if(top_diff<lowest_top) lowest_top = top_diff;
  }
    
  tab_height_ = lowest_top;
  button_width_ = tab_height_;
    
  return 0;
}

int Fl_Scroll_Tabs::can_scroll_left() const {
  return offset>0;
}

int Fl_Scroll_Tabs::can_scroll_right() const {
  const long max_offset = (LABEL_WIDTH*children())-w()+(button_width_<<1);
  return (max_offset>0) && (offset<max_offset);
}

void Fl_Scroll_Tabs::increment_cb() {
  if (can_scroll_left()) {
    offset--;
    redraw();
  }
}

void Fl_Scroll_Tabs::decrement_cb() {
  if (can_scroll_right()) {
    offset++;
    redraw();
  }
}

void Fl_Scroll_Tabs::timeout_cb(void *a) {
  Fl_Scroll_Tabs *that = static_cast<Fl_Scroll_Tabs *>(a);

  for (int i = 0; i<TAB_SCROLL; i++)
    that->do_scroll_cb();

  Fl::add_timeout(REPEAT, timeout_cb, that);
}

int Fl_Scroll_Tabs::ensure_value() {
  int w = children();
    
  if (w==0) {
    value_ = NULL;
  }
  else if (value_==NULL) {
    value_ = child(0);
    w = 0;
  }
  else {
    const int n = w;
    int i = 0;
    for(Fl_Widget *const *childs=array(); i < n; childs++, i++){
      if(*childs==value_)
        w = i;
      else
        (*childs)->hide();
    }
        
    if(w==children())
      value_=child(w-1);
        
    if(!value_->visible())
      value_->show(); 
  }
  return w;
}

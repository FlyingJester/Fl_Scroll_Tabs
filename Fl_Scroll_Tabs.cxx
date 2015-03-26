#include "Fl_Scroll_Tabs.H"
#include <FL/fl_draw.H>
#include <FL/Fl.H>

#define LABEL_WIDTH 128
#define TAB_SCROLL 8
#define MINIMUM_TAB_HEIGHT 16

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

Fl_Widget *Fl_Scroll_Tabs::push() const {
  return value_;
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
    if (Fl::event_y()<y()+tab_height_) {
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
          if (inside_left_button || inside_right_button) {
            Fl::add_timeout(INITIALREPEAT, timeout_cb, this);
             return 1;
          }
        }
        if (e==FL_RELEASE) {
        
          // We need to redraw the scroll button
          if (pressed_!=-1)
            redraw();

          pressed_ = -1;

          if (inside_left_button || inside_right_button)
            Fl::remove_timeout(timeout_cb, this);
          else { // Mouse is inside the tab bar itself
            const int effective_x = Fl::event_x()+offset-button_width_,
              x_inside_tab = effective_x%LABEL_WIDTH,
              selected_tab = effective_x/LABEL_WIDTH;
                        
            if (children()<=selected_tab)
              return 1;
            if (selected_tab<0)
              return 1;
            else if (closebutton_ && (LABEL_WIDTH-x_inside_tab<16)) {
              if (close_callback_)
                close_callback_(child(selected_tab), close_callback_arg_);
              remove(child(selected_tab));                        
              ensure_value();
              redraw();
            }
            else
              push(selected_tab);
                    
            return 1;
          }
        }
      }
    }

  return Fl_Group::handle(e);
}

void Fl_Scroll_Tabs::draw() {
    
  // Draw our box
  fl_draw_box(box(), x(), y(), w(), h(), color());
    
  if(!children())
    return;

  calculate_tab_sizes();
    
  // Draw the left and right buttons.
  fl_draw_box((pressed_==1)?FL_DOWN_FRAME:FL_UP_FRAME, x(),        y(), button_width_, tab_height_, color());
  fl_draw_box((pressed_==2)?FL_DOWN_FRAME:FL_UP_FRAME, x()+w()-button_width_, y(), button_width_, tab_height_, color());
    
  {
    // Draw the arrows
    const unsigned arrow_offsets = 4;
        
    fl_color(can_scroll_left()?labelcolor():FL_INACTIVE_COLOR);
    fl_polygon(x()+button_width_-arrow_offsets, y()+arrow_offsets, 
      x()+arrow_offsets, y()+(tab_height_>>1),
      x()+button_width_-arrow_offsets, y()+tab_height_-arrow_offsets);

    fl_color(can_scroll_right()?labelcolor():FL_INACTIVE_COLOR);
    fl_polygon(x()+w()-button_width_+arrow_offsets, y()+arrow_offsets, 
      x()+w()-arrow_offsets, y()+(tab_height_>>1),
      x()+w()-button_width_+arrow_offsets, y()+tab_height_-arrow_offsets);
  }

  {
    fl_font(labelfont(), labelsize());
    
    // Clip to the tab bar
    fl_push_clip(x()+button_width_, y(), w()-(button_width_<<1), tab_height_);
        
    const int font_offset = (tab_height_+fl_height())>>1;
        
    // Draw children.
    for (int i = 0; i<children(); i++) {
      const int that_x = x()+(i*LABEL_WIDTH)-offset+button_width_+2;
            
      if (fl_not_clipped(that_x, y(), LABEL_WIDTH, tab_height_)==0)
        continue;
      // Draw the frame for the tab panel
      if (child(i)==value_)
        fl_draw_box(FL_DOWN_BOX, that_x-2, y(), LABEL_WIDTH, tab_height_+4, selection_color());
      else
        fl_draw_box(FL_UP_BOX, that_x, y()+2, LABEL_WIDTH-4, tab_height_+2, color());
                        
      // Draw the tab title
      fl_push_clip(that_x, y(), LABEL_WIDTH-tab_height_, tab_height_);
      fl_color(labelcolor());
      fl_draw(child(i)->label(), that_x, y()+font_offset);
      fl_pop_clip();

      if (closebutton_) {
        // Draw the close button
        if(child(i)==value_)
          fl_draw_box(FL_THIN_DOWN_FRAME, that_x+LABEL_WIDTH-button_width_-1, y()+3, button_width_-4, tab_height_-4, color());
        else
          fl_draw_box(FL_THIN_DOWN_FRAME, that_x+LABEL_WIDTH-button_width_-3, y()+4, button_width_-4, tab_height_-4, color());
        // Not really sure how we should do the little 'x', so just write an 'x' string for now.
        fl_color(labelcolor());
        fl_draw("x", 1, that_x+LABEL_WIDTH-button_width_+2, y()+font_offset);
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

// UiEventHandler.hpp


#ifndef UIEVENTHANDLER_HPP
#define UIEVENTHANDLER_HPP

#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif

namespace app {

  using boost::shared_ptr;

  class IUiEventHandler {
  public:
    typedef shared_ptr<IUiEventHandler> ptr;
    virtual ~IUiEventHandler() {}
    
    virtual void onButtonCopy() = 0;
    virtual void onButtonPaste() = 0;
  };

} // namespace

#endif


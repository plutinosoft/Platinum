%module(directors="1") media_renderer
%{
#include "media_renderer.h"
%}

%feature("director") MediaRendererDelegate;
%feature("director") MediaRenderer;

%include "media_renderer.h"

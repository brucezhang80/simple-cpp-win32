// Foobar.cpp : Implementation of CFoobar

#include "stdafx.h"
#include "Foobar.h"

// CFoobar

STDMETHODIMP_(INT) CFoobar::DoSomethingUseful(INT a, INT b)
{
  return a + b;
}

// Creates a todo

var doc = dte.ActiveDocument;


var email = "binazersy@gmail.com"
var date = new Date();
var year = date.getYear();
var month = date.getMonth() + 1;
var day = date.getDate();
var datestr = year + "-";
if (month > 9)
    datestr = datestr + month + "-";
else
    datestr = datestr + "0" + month + "-";
if (day > 9)
    datestr = datestr + day;
else
    datestr = datestr + "0" + day;


if (dte.UndoContext.IsOpen)
    dte.UndoContext.Close();

dte.UndoContext.Open("bina_todo");

//
doc.Selection.Insert("//! @todo " + datestr + " ");

dte.UndoContext.Close();
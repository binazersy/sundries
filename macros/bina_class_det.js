// Creates a class header

var doc = dte.ActiveDocument;

var cmt = "//!";
var cmts = cmt + " ";
var divider = "---------------------------------------------------------";
var tabstr = "\t";

var filename = doc.Name;
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

dte.UndoContext.Open("bina_class");

//
doc.Selection.Insert(cmt + divider);
doc.Selection.NewLine();
doc.Selection.Insert(cmts + "@brief ");
doc.Selection.NewLine();
doc.Selection.Insert(cmts + "@details ");
doc.Selection.NewLine();
doc.Selection.Insert(cmts + "@author " + email);
doc.Selection.NewLine();
//doc.Selection.Insert(cmts + "@date " + datestr);
//doc.Selection.NewLine();
doc.Selection.Insert(cmts + "@version " + "0.0.0.1");
doc.Selection.NewLine();
doc.Selection.Insert(cmt + divider);
doc.Selection.NewLine();
doc.Selection.NewLine();
doc.Selection.Insert(cmts);


dte.UndoContext.Close();
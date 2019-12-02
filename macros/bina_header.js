/// <reference path="C:\Users\Laki\AppData\Local\Microsoft\VisualStudio\15.0_dbaf7a17\Macros\dte.js" />
// Creates a header

var doc = dte.ActiveDocument;

var divider = "=======================================================================";
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

dte.UndoContext.Open("bina_header");

// Go to start of document
doc.Selection.StartOfDocument(false);


doc.Selection.SelectLine();
var cmp = "/**=======";
var startstr = doc.Selection.Text.substr(0, cmp.length);

if (startstr != cmp) {
    // Start header
    doc.Selection.Insert("/**" + divider, 1);
    doc.Selection.NewLine();

    // file name
    doc.Selection.NewLine();
    doc.Selection.Insert(tabstr + filename, 1);
    doc.Selection.NewLine();
    doc.Selection.NewLine();

    // author
    doc.Selection.StartOfLine();
    doc.Selection.Insert(tabstr + tabstr + "@author: " + email, 1);
    doc.Selection.NewLine();

    // date
    //doc.Selection.StartOfLine();
    //doc.Selection.Insert(tabstr + tabstr + "@date: " + datestr, 1);
    //doc.Selection.NewLine();
    //doc.Selection.NewLine();

    // Close header
    doc.Selection.StartOfLine();
    doc.Selection.Insert(divider + "**/", 1);

    doc.Selection.NewLine();
}

dte.UndoContext.Close();
/// <reference path="C:\Users\Laki\AppData\Local\Microsoft\VisualStudio\15.0_dbaf7a17\Macros\dte.js" />
// Creates a method header

var doc = dte.ActiveDocument;

var divider = "---------------------------------------------------------";
var tabstr = "\t";

if (dte.UndoContext.IsOpen)
    dte.UndoContext.Close();

dte.UndoContext.Open("bina_method");


doc.Selection.SelectLine();
var decl = doc.Selection.Text.toString();
//doc.Selection.StartOfLine();

// find parameter eg) "(int a, int b)"
var re = /\(.*\)/;
var m = decl.match(re);

if (m && m[0].length >= 2) {    // at least "()"
    // remove brace
    var ex_brace = m[0].substr(1, m[0].length - 2);

    // split parameters
    var params = new Array();
    var spl = ex_brace.split(",");
    for (var i in spl) {
        var tok = spl[i].split(/[\s+|\*|\&]/);
        if (tok.length > 1)
            params.push(tok[tok.length - 1]);   // insert name
    }

    doc.Selection.LineUp();
    doc.Selection.NewLine();

    doc.Selection.Insert("//! " + "@brief ");
    doc.Selection.NewLine();

    for (var i in params) {
        doc.Selection.Insert("//! " + "@param " + params[i] + tabstr);
        doc.Selection.NewLine();
    }

    // if not void, has return
    if (!decl.match("void")) {
        doc.Selection.Insert("//! " + "@return ");
        doc.Selection.NewLine();
    }
    doc.Selection.Insert("//!" + divider);
    doc.Selection.NewLine();
    doc.Selection.Insert("//!");
    //doc.Selection.NewLine();
}

dte.UndoContext.Close();
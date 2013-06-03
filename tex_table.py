#!/usr/bin/env python
import sys
import json

def format_table(table, caption, rows, cols):
    print "\\begin{center}\\begin{table}[hbp]\\centering"
    print "\\caption{{{0}}}".format(caption.encode("utf-8"))
    print "\\begin{tabular}{|l" + "|c" * (len(cols)-1) + "|}"
    print "\\hline"

    print " & ".join(str(x) for x in cols), "\\\\ \\hline"

    max_chi = 0
    for r in table:
        for x in r:
            if x:
                max_chi = max(max_chi, abs(float(x[1])))
    def cell(x, max_chi):
        if not x:
            return "-"
        return "\\cellcolor[gray]{{{0:.2f}}} $\\frac{{{1}}}{{{2}}}$".format(
                1 - 0.3 * float(abs(x[1])) / max_chi,
                int(x[1]), int(x[0]))

    for first_cell, row in zip(rows, table):
        print "{0} & {1} \\\\ \\hline".format(
                first_cell, " & ".join(cell(x, max_chi) for x in row))
    print "\\end{tabular}"
    print "\\end{table}\\end{center}"



if __name__ == "__main__":
    data = json.loads(sys.stdin.read())
    format_table(data["values"], data["caption"], data["rows"], data["cols"])

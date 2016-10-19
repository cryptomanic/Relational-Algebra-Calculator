---
title: "Relational Algebra Calculus"
author: "Deepak Yadav"
date: "17 October 2016"
output: pdf_document
---

**How to use Relation Algebra Calculator**

**1. select ($)**
      
      syntax: $(table)(conditions)
      conditions support <, >, =, &, !, |, +, -, * operators only
      <= is not supported but same is possible using <, &, =
      similarly >= using >, &, =
      and != using !, =
**2. project (#)**
      
      syntax: #(table)(column1, column2, column3, ....)
**3. union (U)**
      
      syntax: U(first_table)(second_table)      
**4. difference (D)**
      
      syntax: D(first_table)(second_table)   
**5. rename (R)**
      
      syntax: R(old_table)(new_table_name[, new_column1_name, ...])      
**6. cartesian product (C)**
      
      syntax: C(first_table)(second_table)     
**Calculator also support nested queries**

**project inside select**

      $(#(table)(column1, column2, column3, ....))(conditions)
**select inside project**
      
      #($(table)(conditions))(column1, column2, column3, ....)
**nesting can be complex**
    
      C(R(U(#(table1)(column1, column2, column3, ....))($(table3)(conditions)))
       (new_table_name[, new_column1_name, ...]))
       (R(D($(table2)(conditions))(#(table4)(column1, column2, column3, ....)))
       (new_table_name[, new_column1_name, ...]))
**Steps to run calculator**

      1. open the terminal
      2. change the directory to Relational-Algebra-Calculator
      3. g++ -std=c++11 nested_query.cpp -o output.out
      4. ./output.out


set contexts {
    twocubes
    surfmachineposter mesh interordinal3
    bastiansthesis call-graph1 chain-4 chain-5 fig8 freese2 freese
    knives n5 n5ext rjthesis second
    contra_call-graph1 contra_freese
    contra_knives contra_n5 2mutex b4 b5 
<<<<<<< create_makefile.tcl
    ordinal_six 2n3s fig16
=======
    
    ordinal_six 2n3s fig16 bible

>>>>>>> 1.7
}

set distrib_contexts { 
  surfmachineposter mesh bastiansthesis chain-4 chain-5
  contra_call-graph1 contra_freese contra_knives
  contra_n5 b4 b5 ordinal_six bible
  contra_call-graph1 contra_chain-5 contra_freese contra_knives
}

set Contexts(all) $contexts
set Contexts(distrib) $distrib_contexts
set Type(all) plain
set Type(distrib) distrib

set removed_contexts {
    tt0047396 tt0057012 tt0076759 tt0080684 tt0082971
    tt0087469  tt0118298  tt0146316  tt0295178
}

puts "HOME=/home/rcole"
puts "CB=\$(HOME)/Source/griff/cass_browser"
puts "SARL=\$(HOME)/Source/sarl_caml"
puts ""
puts "TEST_LAYOUT=\$(SARL)/test_layout.exe"

foreach cxt $contexts {
  lappend cxtlist $cxt.cxt
}

puts "are_contexts_distributive.txt: \$(CB)/calculate_is_distributive.rb $cxtlist"
puts "\truby -I \$(CB) \$(CB)/calculate_is_distributive.rb $cxtlist > are_contexts_distributive.txt"
puts ""

foreach y { "all" "distrib" } {
    set layout_type $Type($y)
    foreach x $Contexts($y) {
        if { 1==[string match "all" $y] } {
            set y ""
        }
        if { 1==[string match "distrib" $y] } {
            set y "-distrib"
        }
        
        puts "$x$y-diagrams.xml: \$(TEST_LAYOUT) $x.cxt"
        puts "\t\$(TEST_LAYOUT) $layout_type $x $x$y-diagrams.xml"
        puts ""
        
        puts "$x$y-stats.xml: \$(CB)/calculate_stats_from_xml.rb  $x$y-diagrams.xml"
        puts "\truby -I \$(CB) \$(CB)/calculate_stats_from_xml.rb $x$y-diagrams.xml $x$y-stats.xml"
        puts ""
        
        lappend rank_contexts_$layout_type $x$y-rank.cxt
        puts "$x$y-rank.cxt: \$(CB)/calculate_ranks.rb $x$y-stats.xml"
        puts "\truby -I \$(CB)/ \$(CB)/calculate_ranks.rb $x$y-stats.xml $x$y-rank.cxt $x$y-rank.xml"
        puts ""
        
        lappend xml_ranks_$layout_type $x$y-rank.xml
        puts "$x$y-rank.xml: \$(CB)/calculate_ranks.rb $x$y-stats.xml"
        puts "\truby -I \$(CB)/ \$(CB)/calculate_ranks.rb $x$y-stats.xml $x$y-rank.cxt $x$y-rank.xml"
        puts ""
        
        puts "$x$y-best_diagrams.txt: \$(CB)/bottom_cochain.rb $x$y-diagrams.xml $x$y-rank.cxt"
        puts "\truby -I \$(CB)/ \$(CB)/bottom_cochain.rb $x$y-diagrams.xml $x$y-rank.cxt > $x$y-best_diagrams.txt"
        puts ""
         
        lappend best_diagrams_$layout_type $x$y-best_diagrams.png
        puts "$x$y-best_diagrams.png: \$(CB)/draw_diagram_collage_from_filelist.rb $x$y-best_diagrams.txt"
        puts "\truby -I \$(CB)/ \$(CB)/draw_diagram_collage_from_filelist.rb $x$y-best_diagrams.txt $x$y-best_diagrams.png"
        puts "\tif \[ ! -e $x$y-best_diagrams.png \] ; then ln -s $x$y-best_diagrams.png.0 $x$y-best_diagrams.png ; fi"
        puts ""
        
        lappend svm_training_$layout_type $x$y-best_diagrams.train
        puts "$x$y-best_diagrams.train: \$(CB)/calculate_svm_training_data.rb $x$y-judgements.txt $x$y-rank.xml $x$y-diagrams.xml"
        puts "\truby -I \$(CB)/ \$(CB)/calculate_svm_training_data.rb $x$y-judgements.txt > $x$y-best_diagrams.train"
        puts ""
        
        lappend vs_training_$layout_type $x$y-best_diagrams-vs.train
        puts "$x$y-best_diagrams-vs.train: \$(CB)/calculate_version_space_data.rb $x$y-judgements.txt $x$y-rank.xml $x$y-diagrams.xml"
        puts "\truby -I \$(CB)/ \$(CB)/calculate_version_space_data.rb $x$y-judgements.txt > $x$y-best_diagrams-vs.train"
        puts ""
        
        lappend ada_training_$layout_type $x$y-best_diagrams-ada.train
        puts "$x$y-best_diagrams-ada.train: \$(CB)/calculate_adaboost_data.rb $x$y-judgements.txt $x$y-rank.xml $x$y-diagrams.xml"
        puts "\truby -I \$(CB)/ \$(CB)/calculate_adaboost_data.rb $x$y-judgements.txt > $x$y-best_diagrams-ada.train"
        puts ""

        lappend targets simple_training_$layout_type 
        lappend simple_training_$layout_type $x$y-best_diagrams-simple.train
        puts "$x$y-best_diagrams-simple.train: \$(CB)/calculate_simple_data.rb $x$y-judgements.txt $x$y-rank.xml $x$y-diagrams.xml"
        puts "\truby -I \$(CB)/ \$(CB)/calculate_simple_data.rb $x$y-judgements.txt > $x$y-best_diagrams-simple.train"
        puts ""

        lappend judgements_$layout_type $x$y-judgements.txt
    }

}
    
foreach layout_type { plain distrib } {
    foreach target { 
        best_diagrams xml_ranks rank_contexts vs_training 
        svm_training ada_training simple_training
    } {
        puts "$target\_$layout_type: [subst \$$target\_$layout_type]"
        puts ""
    }

}


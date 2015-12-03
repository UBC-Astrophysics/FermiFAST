BEGIN { commenton=0; }
{
    if ($1=="*") {
	commenton=!commenton;
	printf("# %s\n",$0);
    } else { 
	if (commenton) {
	    printf("# %s\n",$0);
	} else {
	    print;
	}
    }
}

OUTPUT_FORMAT("binary")
OUTPUT(krnl)
STARTUP(kernel.o)
LoadAddress = 0x00010000;

SECTIONS
{
	.text LoadAddress : AT(LoadAddress)
	{
		*(.text)
	}
	.data : {*(.data)}
	.bss : {*(.bss)}
}
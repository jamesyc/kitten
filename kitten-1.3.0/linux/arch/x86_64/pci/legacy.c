/*
 * legacy.c - traditional, old school PCI bus probing
 */
#include <linux/init.h>
#include <linux/pci.h>
#include "pci.h"

/*
 * Discover remaining PCI buses in case there are peer host bridges.
 * We use the number of last PCI bus provided by the PCI BIOS.
 */
static void __devinit pcibios_fixup_peer_bridges(void)
{
	int n, devfn;
	long node;

	if (pcibios_last_bus <= 0 || pcibios_last_bus > 0xff)
		return;
	DBG("PCI: Peer bridge fixup\n");

	for (n=0; n <= pcibios_last_bus; n++) {
		u32 l;
		if (pci_find_bus(0, n))
			continue;
		node = get_mp_bus_to_node(n);
		for (devfn = 0; devfn < 256; devfn += 8) {
			if (!raw_pci_read(0, n, devfn, PCI_VENDOR_ID, 2, &l) &&
			    l != 0x0000 && l != 0xffff) {
				DBG("Found device at %02x:%02x [%04x]\n", n, devfn, l);
				printk(KERN_INFO "PCI: Discovered peer bus %02x\n", n);
				pci_scan_bus_on_node(n, &pci_root_ops, node);
				break;
			}
		}
	}
}

static int __init pci_legacy_init(void)
{
	if (!raw_pci_ops) {
		printk(KERN_INFO "PCI: System does not support PCI\n");
		return 0;
	}

	if (pcibios_scanned++)
		return 0;

	pci_root_bus = pcibios_scan_root(0);
	if (pci_root_bus)
		pci_bus_add_devices(pci_root_bus);

	pcibios_fixup_peer_bridges();

	return 0;
}

int __init pci_subsys_init(void)
{
#ifdef CONFIG_X86_NUMAQ
	pci_numaq_init();
#endif
#ifdef CONFIG_ACPI
	pci_acpi_init();
#endif
#ifdef CONFIG_X86_VISWS
	pci_visws_init();
#endif
	pci_legacy_init();
	pcibios_irq_init();
	pcibios_init();

	return 0;
}
subsys_initcall(pci_subsys_init);

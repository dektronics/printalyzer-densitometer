<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="node()|@*">
  <xsl:copy>
    <xsl:apply-templates select="node()|@*"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="/export/components/comp/fields/field/@name[.='Manufacturer PN']">
  <xsl:attribute name="name">manf#</xsl:attribute>
</xsl:template>

<xsl:template match="/export/components/comp/fields/field[../field[@name='Supplier' and text()='Digi-Key']]/@name[.='Supplier PN']">
  <xsl:attribute name="name">digikey#</xsl:attribute>
</xsl:template>

<xsl:template match="/export/components/comp/fields/field[../field[@name='Supplier' and text()='Mouser']]/@name[.='Supplier PN']">
  <xsl:attribute name="name">mouser#</xsl:attribute>
</xsl:template>

</xsl:stylesheet>
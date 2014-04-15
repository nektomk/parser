<?xml version="1.0" encoding="UTF-8"?>
<!-- конверсия синтаксического разбора кода в html -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<!-- <xsl:output method="xml"
	doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
	doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>
-->
<!-- заголовок html -->
<xsl:template match="/">
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
		<link rel="stylesheet" type="text/css" href="lang.css"/>
	</head>
	<body>
		<xsl:apply-templates/>
	</body>
</html>
</xsl:template>
<!-- выражение языка -->
<xsl:template match="expr">
	<span class="expr">
		<xsl:apply-templates/>
	</span>
</xsl:template>
<!-- слово языка -->
<xsl:template match="word">
	<span class="word">
		<xsl:apply-templates/>
	</span>
</xsl:template>

<!-- корневой тег "code" -->
<xsl:template match="code">
<pre>
	<xsl:apply-templates/>
</pre>
</xsl:template>

<!-- цифровая константа -->
<xsl:template match="numeric">
	<span class="numeric">
		<xsl:apply-templates/>
	</span>
</xsl:template>
<xsl:template match="double">
	<span class="double">
		<xsl:apply-templates/>
	</span>
</xsl:template>
<xsl:template match="long">
	<span class="long">
		<xsl:apply-templates/>
	</span>
</xsl:template>

<!-- строчная константа -->
<xsl:template match="string">
	<span class="string">
		<xsl:apply-templates/>
	</span>
</xsl:template>
<xsl:template match="esc">
	<span class="esc">
		<xsl:apply-templates/>
	</span>
</xsl:template>

<!-- список {} -->
<xsl:template match="list">
	<span class="list">
		<xsl:apply-templates/>
	</span>
</xsl:template>

<!-- подстановка команды [] -->
<xsl:template match="command">
	<span class="command">
		<xsl:apply-templates/>
	</span>
</xsl:template>

<xsl:template match="macro">
	<span class="macro">
		<xsl:apply-templates/>
	</span>
</xsl:template>

<!-- подстановка переменной $var -->
<xsl:template match="var">
	<span class="var">
		<xsl:apply-templates/>
	</span>
</xsl:template>

<!-- комментарий # -->
<xsl:template match="comment">
	<span class="comment">
		<xsl:apply-templates/>
	</span>
</xsl:template>

<!-- вывод в консоль -->
<xsl:template match="print">
	<span class="print">
		<xsl:apply-templates/>
	</span>
</xsl:template>

<!-- сообщения об ошибке -->
<xsl:template match="error">
	<span class="error">
		<xsl:apply-templates/>
	</span>
</xsl:template>
</xsl:stylesheet>


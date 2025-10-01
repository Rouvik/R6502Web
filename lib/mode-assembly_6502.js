define(
  "ace/mode/assembly_6502_highlight_rules",
  [
    "require",
    "exports",
    "module",
    "ace/lib/oop",
    "ace/mode/text_highlight_rules",
  ],
  function (s, e, t) {
    "use strict";
    function d() {
      (this.$rules = {
        start: [
          {
            token: "constant.character.hexadecimal.assembly",
            regex: " \\$[0-9a-fA-F]+ *",
            caseInsensitive: !0,
          },
          {
            token: "keyword.control.assembly",
            regex:
              "\\b(?:brk|ora|jam|slo|nop|ora|asl|slo|php|ora|asl|anc|nop|ora|asl|slo|bpl|ora|jam|slo|nop|ora|asl|slo|clc|ora|nop|slo|nop|ora|asl|slo|jsr|and|jam|rla|bit|and|rol|rla|plp|and|rol|anc|bit|and|rol|rla|bmi|and|jam|rla|nop|and|rol|rla|sec|and|nop|rla|nop|and|rol|rla|rti|eor|jam|sre|nop|eor|lsr|sre|pha|eor|lsr|alr|jmp|eor|lsr|sre|bvc|eor|jam|sre|nop|eor|lsr|sre|cli|eor|nop|sre|nop|eor|lsr|sre|rts|adc|jam|rra|nop|adc|ror|rra|pla|adc|ror|arr|jmp|adc|ror|rra|bvs|adc|jam|rra|nop|adc|ror|rra|sei|adc|nop|rra|nop|adc|ror|rra|nop|sta|nop|sax|sty|sta|stx|sax|dey|nop|txa|ane|sty|sta|stx|sax|bcc|sta|jam|sha|sty|sta|stx|sax|tya|sta|txs|tas|shy|sta|shx|sha|ldy|lda|ldx|lax|ldy|lda|ldx|lax|tay|lda|tax|lxa|ldy|lda|ldx|lax|bcs|lda|jam|lax|ldy|lda|ldx|lax|clv|lda|tsx|las|ldy|lda|ldx|lax|cpy|cmp|nop|dcp|cpy|cmp|dec|dcp|iny|cmp|dex|sbx|cpy|cmp|dec|dcp|bne|cmp|jam|dcp|nop|cmp|dec|dcp|cld|cmp|nop|dcp|nop|cmp|dec|dcp|cpx|sbc|nop|isc|cpx|sbc|inc|isc|inx|sbc|nop|usb|cpx|sbc|inc|isc|beq|sbc|jam|isc|nop|sbc|inc|isc|sed|sbc|nop|isc|nop|sbc|inc|isc)\\b",
            caseInsensitive: !0,
          },
          {
            token: "variable.parameter.register.assembly",
            regex:
              "(?:A|X|Y)",
            caseInsensitive: !0,
          },
          {
            token: "constant.character.hexadecimal.assembly",
            regex: " #\\$[0-9a-zA-F]+ *",
            caseInsensitive: !0,
          },
          {
            token: "constant.character.decimal.assembly",
            regex: " #[0-9]+ *",
          },
          {
            token: "constant.character.decimal.assembly",
            regex: "\\b[0-9]+\\b",
          },
          
          
          {
            token: "constant.character.hexadecimal.assembly",
            regex: "\\b[A-F0-9]+h\\b",
            caseInsensitive: !0,
          },
          {
            token: "editor.string",
            regex: "\\b[a-zA-Z_][a-zA-Z0-9_]*:",
            caseInsensitive: !0
          },
          {
            token: "editor.string",
            regex: "\\b\.[a-zA-Z]+",
            caseInsensitive: !0
          },
          { token: "string.assembly", regex: /'([^\\']|\\.)*'/ },
          { token: "string.assembly", regex: /"([^\\"]|\\.)*"/ },
          {
            token: "support.function.directive.assembly",
            regex: "^\\[",
            push: [
              {
                token: "support.function.directive.assembly",
                regex: "\\]$",
                next: "pop",
              },
              { defaultToken: "support.function.directive.assembly" },
            ],
          },
          {
            token: [
              "support.function.directive.assembly",
              "support.function.directive.assembly",
              "entity.name.function.assembly",
            ],
            regex: "(^struc)( )([_a-zA-Z][_a-zA-Z0-9]*)",
          },
          {
            token: "support.function.directive.assembly",
            regex: "^endstruc\\b",
          },
          {
            token: [
              "support.function.directive.assembly",
              "entity.name.function.assembly",
              "support.function.directive.assembly",
              "constant.character.assembly",
            ],
            regex: "^(%macro )([_a-zA-Z][_a-zA-Z0-9]*)( )([0-9]+)",
          },
          { token: "support.function.directive.assembly", regex: "^%endmacro" },
          {
            token: [
              "text",
              "support.function.directive.assembly",
              "text",
              "entity.name.function.assembly",
            ],
            regex:
              "(\\s*)(%define|%xdefine|%idefine|%undef|%assign|%defstr|%strcat|%strlen|%substr|%00|%0|%rotate|%rep|%endrep|%include|\\$\\$|\\$|%unmacro|%if|%elif|%else|%endif|%(?:el)?ifdef|%(?:el)?ifmacro|%(?:el)?ifctx|%(?:el)?ifidn|%(?:el)?ifidni|%(?:el)?ifid|%(?:el)?ifnum|%(?:el)?ifstr|%(?:el)?iftoken|%(?:el)?ifempty|%(?:el)?ifenv|%pathsearch|%depend|%use|%push|%pop|%repl|%arg|%stacksize|%local|%error|%warning|%fatal|%line|%!|%comment|%endcomment|__NASM_VERSION_ID__|__NASM_VER__|__FILE__|__LINE__|__BITS__|__OUTPUT_FORMAT__|__DATE__|__TIME__|__DATE_NUM__|_TIME__NUM__|__UTC_DATE__|__UTC_TIME__|__UTC_DATE_NUM__|__UTC_TIME_NUM__|__POSIX_TIME__|__PASS__|ISTRUC|AT|IEND|BITS 16|BITS 32|BITS 64|USE16|USE32|__SECT__|ABSOLUTE|EXTERN|GLOBAL|COMMON|CPU|FLOAT)\\b( ?)((?:[_a-zA-Z][_a-zA-Z0-9]*)?)",
            caseInsensitive: !0,
          },
          {
            token: "support.function.directive.assembly",
            regex:
              "\\b(?:d[bwdqtoy]|res[bwdqto]|equ|times|align|alignb|sectalign|section|ptr|byte|word|dword|qword|incbin)\\b",
            caseInsensitive: !0,
          },
          {
            token: "entity.name.function.assembly",
            regex: "^\\s*%%[\\w.]+?:$",
          },
          {
            token: "entity.name.function.assembly",
            regex: "^\\s*%\\$[\\w.]+?:$",
          },
          { token: "entity.name.function.assembly", regex: "^[\\w.]+?:" },
          { token: "entity.name.function.assembly", regex: "^[\\w.]+?\\b" },
          { token: "comment.assembly", regex: ";.*$" },
        ],
      }),
        this.normalizeRules();
    }
    var p = s("../lib/oop"),
      s = s("./text_highlight_rules").TextHighlightRules;
    (d.metaData = {
      fileTypes: ["asm"],
      name: "Assembly x86",
      scopeName: "source.assembly",
    }),
      p.inherits(d, s),
      (e.AssemblyX86HighlightRules = d);
  }
),
  define(
    "ace/mode/folding/coffee",
    [
      "require",
      "exports",
      "module",
      "ace/lib/oop",
      "ace/mode/folding/fold_mode",
      "ace/range",
    ],
    function (s, e, t) {
      "use strict";
      var d = s("../../lib/oop"),
        p = s("./fold_mode").FoldMode,
        c = s("../../range").Range,
        s = (e.FoldMode = function () {});
      d.inherits(s, p),
        function () {
          (this.commentBlock = function (s, e) {
            var t = /\S/,
              d = (a = s.getLine(e)).search(t);
            if (-1 != d && "#" == a[d]) {
              for (
                var p, d = a.length, n = s.getLength(), i = e, o = e;
                ++e < n;

              ) {
                var a,
                  r = (a = s.getLine(e)).search(t);
                if (-1 != r) {
                  if ("#" != a[r]) break;
                  o = e;
                }
              }
              return i < o
                ? ((p = s.getLine(o).length), new c(i, d, o, p))
                : void 0;
            }
          }),
            (this.getFoldWidgetRange = function (s, e, t) {
              var d = this.indentationBlock(s, t);
              return d || ((d = this.commentBlock(s, t)) ? d : void 0);
            }),
            (this.getFoldWidget = function (s, e, t) {
              var d = s.getLine(t),
                p = d.search(/\S/),
                n = s.getLine(t + 1),
                i = s.getLine(t - 1),
                o = i.search(/\S/),
                a = n.search(/\S/);
              if (-1 == p)
                return (
                  (s.foldWidgets[t - 1] = -1 != o && o < a ? "start" : ""), ""
                );
              if (-1 == o) {
                if (p == a && "#" == d[p] && "#" == n[p])
                  return (
                    (s.foldWidgets[t - 1] = ""),
                    (s.foldWidgets[t + 1] = ""),
                    "start"
                  );
              } else if (
                o == p &&
                "#" == d[p] &&
                "#" == i[p] &&
                -1 == s.getLine(t - 2).search(/\S/)
              )
                return (
                  (s.foldWidgets[t - 1] = "start"), (s.foldWidgets[t + 1] = "")
                );
              return (
                (s.foldWidgets[t - 1] = -1 != o && o < p ? "start" : ""),
                p < a ? "start" : ""
              );
            });
        }.call(s.prototype);
    }
  ),
  define(
    "ace/mode/assembly_6502",
    [
      "require",
      "exports",
      "module",
      "ace/lib/oop",
      "ace/mode/text",
      "ace/mode/assembly_6502_highlight_rules",
      "ace/mode/folding/coffee",
    ],
    function (s, e, t) {
      "use strict";
      function d() {
        (this.HighlightRules = i),
          (this.foldingRules = null),
          (this.$behaviour = this.$defaultBehaviour);
      }
      var p = s("../lib/oop"),
        n = s("./text").Mode,
        i = s("./assembly_6502_highlight_rules").AssemblyX86HighlightRules,
        o = s("./folding/coffee").FoldMode;
      p.inherits(d, n),
        function () {
          (this.lineCommentStart = [";"]), (this.$id = "ace/mode/assembly_6502");
        }.call(d.prototype),
        (e.Mode = d);
    }
  ),
  window.require(["ace/mode/assembly_6502"], function (s) {
    "object" == typeof module &&
      "object" == typeof exports &&
      module &&
      (module.exports = s);
  });

#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include <pbcopper/cli2/CLI.h>
#include <pbcopper/cli2/Option.h>
#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/Results.h>
#include <pbcopper/cli2/internal/HelpMetrics.h>
#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>
#include <pbcopper/utility/OStreamRedirect.h>

#include "PbcopperTestData.h"

// clang-format off

namespace CLI_v2_CLITests {

const PacBio::CLI_v2::Option RefineTestOption
{
R"({
    "names" : ["test"],
    "description" : "Should see this entry",
    "type" : "boolean"
})"
};

const PacBio::CLI_v2::Option RefineCountOption
{
R"({
    "names" : ["count"],
    "description" : "Should see this entry too",
    "type" : "integer",
    "default" : 5
})"
};

const PacBio::CLI_v2::PositionalArgument Source
{
R"({
    "name" : "source",
    "description" : "Source file to copy.",
    "syntax" : "FILE",
    "required" : false
})"
};


PacBio::CLI_v2::Interface MakeRefineInterface()
{
    PacBio::CLI_v2::Interface refine{"refine", "Remove concatemers and optionally poly-A tails (FL to FLNC)"};
    refine.AddOptions({
        RefineTestOption,
        RefineCountOption});
    refine.Example("isoseq3 refine movie.consensusreadset.xml primers.barcodeset.xml movie.flnc.bam");
    return refine;
}

int RefineRunner(const PacBio::CLI_v2::Results& results)
{
    // "$ isoseq3 refine --test --count 10"

    const bool test = results[RefineTestOption];
    const int count = results[RefineCountOption];
    EXPECT_TRUE(test);
    EXPECT_EQ(10, count);

    return EXIT_SUCCESS;
}

PacBio::CLI_v2::Interface MakeClusterInterface()
{
    PacBio::CLI_v2::Interface cluster{"cluster", "Cluster FLNC reads and generate unpolished transcripts (FLNC to UNPOLISHED"};
    cluster.Example("isoseq3 cluster movie.flnc.bam unpolished.bam");
    return cluster;
}

int ClusterRunner(const PacBio::CLI_v2::Results&)
{
    return EXIT_SUCCESS;
}

PacBio::CLI_v2::Interface MakePolishInterface()
{
    PacBio::CLI_v2::Interface polish{"polish", "Polish transcripts using subreads (UNPOLISHED to POLISHED)"};
    polish.Example("isoseq3 polish unpolished.bam movie.subreadset.xml polished.bam");
    return polish;
}

int PolishRunner(const PacBio::CLI_v2::Results&)
{
    return EXIT_SUCCESS;
}

PacBio::CLI_v2::Interface MakeSummarizeInterface()
{
    PacBio::CLI_v2::Interface summarize{"summarize", "Create barcode overview from transcripts (POLISHED to CSV)"};
    summarize.Example("isoseq3 summarize polished.bam summary.csv");
    return summarize;
}

int SummarizeRunner(const PacBio::CLI_v2::Results&)
{
    return EXIT_SUCCESS;
}

} // namespace CLI_v2_CLITests

TEST(CLI2_CLI, can_print_version)
{
    PacBio::CLI_v2::internal::HelpMetrics::TestingFixedWidth = 80;

    const std::string expectedText{"frobber v3.1\n"};
    const std::vector<std::string> args {
        "frobber", "--version"
    };
    auto runner = [](const PacBio::CLI_v2::Results&)
    {
        return EXIT_SUCCESS;
    };

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const PacBio::CLI_v2::Interface i{"frobber", "Frob all the things", "v3.1"};
    const int result = PacBio::CLI_v2::Run(args, i, runner);

    EXPECT_EQ(EXIT_SUCCESS, result);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI2_CLI, can_print_help)
{
    PacBio::CLI_v2::internal::HelpMetrics::TestingFixedWidth = 80;

    const std::string expectedText{
"frobber - Frob all the things.\n"
"\n"
"Usage:\n"
"  frobber [options]\n"
"\n"
"Options:\n"
"  -h,--help               Show this help and exit.\n"
"  --version               Show application version and exit.\n"
"  -j,--num-threads  INT   Number of threads to use, 0 means autodetection. [0]\n"
"  --log-level       STR   Set log level. Valid choices: (TRACE, DEBUG, INFO,\n"
"                          WARN, FATAL). [WARN]\n"
"  --log-file        FILE  Log to a file, instead of stderr.\n"
"\n"
};

    const std::vector<std::string> args {
        "frobber", "-h"
    };
    auto runner = [](const PacBio::CLI_v2::Results&)
    {
        return EXIT_SUCCESS;
    };

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    const int result = PacBio::CLI_v2::Run(args, i, runner);

    EXPECT_EQ(EXIT_SUCCESS, result);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI2_CLI, can_print_top_help_for_multitoolinterface)
{
    PacBio::CLI_v2::internal::HelpMetrics::TestingFixedWidth = 80;

    const std::string expectedText{R"(isoseq3 - De Novo Transcript Reconstruction

Usage:
  isoseq3 <tool>

  -h,--help    Show this help and exit.
  --version    Show application version and exit.

Tools:
  refine     Remove concatemers and optionally poly-A tails (FL to FLNC)
  cluster    Cluster FLNC reads and generate unpolished transcripts (FLNC to UNPOLISHED
  polish     Polish transcripts using subreads (UNPOLISHED to POLISHED)
  summarize  Create barcode overview from transcripts (POLISHED to CSV)

Examples:
  isoseq3 refine movie.consensusreadset.xml primers.barcodeset.xml movie.flnc.bam
  isoseq3 cluster movie.flnc.bam unpolished.bam
  isoseq3 polish unpolished.bam movie.subreadset.xml polished.bam
  isoseq3 summarize polished.bam summary.csv

Typical workflow:
  1. Generate consensus sequences from your raw subread data
     $ ccs movie.subreads.bam movie.ccs.bam --noPolish --minPasses 1

  2. Generate full-length reads by primer removal and demultiplexing
     $ cat primers.fasta
       >primer_5p
       AAGCAGTGGTATCAACGCAGAGTACATGGGG
       >primer_3p
       AAGCAGTGGTATCAACGCAGAGTAC
     $ lima movie.ccs.bam primers.fasta movie.fl.bam --isoseq --no-pbi

  3. Remove noise from FL reads
     $ isoseq3 refine movie.fl.P5--P3.bam primers.fasta movie.flnc.bam

  4. Cluster consensus sequences to generate unpolished transcripts
     $ isoseq3 cluster movie.flnc.bam unpolished.bam --verbose

  5. Polish transcripts using subreads
     $ isoseq3 polish unpolished.bam movie.subreads.bam polished.bam
)"};

    PacBio::CLI_v2::MultiToolInterface i{"isoseq3", "De Novo Transcript Reconstruction", "3.1.2"};
    i.AddTools({{"refine",    CLI_v2_CLITests::MakeRefineInterface(),    &CLI_v2_CLITests::RefineRunner},
                {"cluster",   CLI_v2_CLITests::MakeClusterInterface(),   &CLI_v2_CLITests::ClusterRunner},
                {"polish",    CLI_v2_CLITests::MakePolishInterface(),    &CLI_v2_CLITests::PolishRunner},
                {"summarize", CLI_v2_CLITests::MakeSummarizeInterface(), &CLI_v2_CLITests::SummarizeRunner}});

    i.HelpFooter(R"(Typical workflow:
  1. Generate consensus sequences from your raw subread data
     $ ccs movie.subreads.bam movie.ccs.bam --noPolish --minPasses 1

  2. Generate full-length reads by primer removal and demultiplexing
     $ cat primers.fasta
       >primer_5p
       AAGCAGTGGTATCAACGCAGAGTACATGGGG
       >primer_3p
       AAGCAGTGGTATCAACGCAGAGTAC
     $ lima movie.ccs.bam primers.fasta movie.fl.bam --isoseq --no-pbi

  3. Remove noise from FL reads
     $ isoseq3 refine movie.fl.P5--P3.bam primers.fasta movie.flnc.bam

  4. Cluster consensus sequences to generate unpolished transcripts
     $ isoseq3 cluster movie.flnc.bam unpolished.bam --verbose

  5. Polish transcripts using subreads
     $ isoseq3 polish unpolished.bam movie.subreads.bam polished.bam)");

    const std::vector<std::string> args {
        "isoseq3", "-h"
    };

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i);
    EXPECT_EQ(EXIT_SUCCESS, result);

    EXPECT_EQ(s.str(), expectedText);
}

TEST(CLI2_CLI, can_print_subtool_help_for_multitoolinterface)
{
    PacBio::CLI_v2::internal::HelpMetrics::TestingFixedWidth = 80;

    const std::string expectedText{R"(refine - Remove concatemers and optionally poly-A tails (FL to FLNC)

Usage:
  refine [options]

  --test                  Should see this entry
  --count           INT   Should see this entry too [5]

  -h,--help               Show this help and exit.
  --version               Show application version and exit.
  -j,--num-threads  INT   Number of threads to use, 0 means autodetection. [0]
  --log-level       STR   Set log level. Valid choices: (TRACE, DEBUG, INFO,
                          WARN, FATAL). [WARN]
  --log-file        FILE  Log to a file, instead of stderr.

)"};

    PacBio::CLI_v2::MultiToolInterface i{"isoseq3", "De Novo Transcript Reconstruction", "3.1.2"};
    i.AddTools({{"refine",    CLI_v2_CLITests::MakeRefineInterface(),    &CLI_v2_CLITests::RefineRunner},
                {"cluster",   CLI_v2_CLITests::MakeClusterInterface(),   &CLI_v2_CLITests::ClusterRunner},
                {"polish",    CLI_v2_CLITests::MakePolishInterface(),    &CLI_v2_CLITests::PolishRunner},
                {"summarize", CLI_v2_CLITests::MakeSummarizeInterface(), &CLI_v2_CLITests::SummarizeRunner}});

    const std::vector<std::string> args {
        "isoseq3", "refine", "-h"
    };

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i);
    EXPECT_EQ(EXIT_SUCCESS, result);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI2_CLI, can_run_from_command_line_args)
{
    const PacBio::CLI_v2::Option MaxNumLines
    {
    R"({
        "names" : ["n"],
        "description" : "Max Number of lines to Copy",
        "type" : "int",
        "default" : 10,
        "tool_contract.option" : {
            "title" : "Maximum Line Count",
            "id" : "max_nlines"
        }
    })"
    };

    const std::vector<std::string> args {
        "frobber", "-n", "27"
    };
    auto runner = [&MaxNumLines](const PacBio::CLI_v2::Results& results)
    {
        const int expectedNumLines = 27;
        const int maxNumLines = results[MaxNumLines];

        EXPECT_EQ(expectedNumLines, maxNumLines);
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.AddOption(MaxNumLines);

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, correctly_interprets_implicit_empty_strings)
{
    const PacBio::CLI_v2::Option TestDefaultEmptyString
    {
    R"({
        "names" : ["test-default-string"],
        "description" : "Nothing to see here.",
        "type" : "string"
    })"
    };

    const PacBio::CLI_v2::Option TestDefaultEmptyFile
    {
    R"({
        "names" : ["test-default-file"],
        "description" : "Nothing to see here.",
        "type" : "file"
    })"
    };

    const PacBio::CLI_v2::Option TestDefaultEmptyDir
    {
    R"({
        "names" : ["test-default-dir"],
        "description" : "Nothing to see here.",
        "type" : "dir"
    })"
    };

    const std::vector<std::string> args {
        "frobber"
    };
    auto runner = [&](const PacBio::CLI_v2::Results& results)
    {
        const std::string shouldBeEmptyString = results[TestDefaultEmptyString];
        const std::string shouldBeEmptyFile = results[TestDefaultEmptyFile];
        const std::string shouldBeEmptyDir = results[TestDefaultEmptyDir];

        EXPECT_TRUE(shouldBeEmptyString.empty());
        EXPECT_TRUE(shouldBeEmptyFile.empty());
        EXPECT_TRUE(shouldBeEmptyDir.empty());
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.AddOptions({
        TestDefaultEmptyString,
        TestDefaultEmptyFile,
        TestDefaultEmptyDir
    });

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_run_multitoolinterface_subtool_from_command_line_args)
{
    PacBio::CLI_v2::MultiToolInterface i{"isoseq3", "De Novo Transcript Reconstruction", "3.1.2"};
    i.AddTools({{"refine",    CLI_v2_CLITests::MakeRefineInterface(),    &CLI_v2_CLITests::RefineRunner},
                {"cluster",   CLI_v2_CLITests::MakeClusterInterface(),   &CLI_v2_CLITests::ClusterRunner},
                {"polish",    CLI_v2_CLITests::MakePolishInterface(),    &CLI_v2_CLITests::PolishRunner},
                {"summarize", CLI_v2_CLITests::MakeSummarizeInterface(), &CLI_v2_CLITests::SummarizeRunner}});

    const std::vector<std::string> args {
        "isoseq3", "refine", "--test", "--count", "10"
    };

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_fetch_default_log_level_and_log_file_from_results)
{
    const PacBio::CLI_v2::Option MaxNumLines
    {
    R"({
        "names" : ["n"],
        "description" : "Max Number of lines to Copy",
        "type" : "int",
        "default" : 10,
        "tool_contract.option" : {
            "title" : "Maximum Line Count",
            "id" : "max_nlines"
        }
    })"
    };

    const std::vector<std::string> args {
        "frobber", "-n", "27"
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.AddOption(MaxNumLines);
    const auto defaultLogLevel = i.DefaultLogLevel();

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    auto runner = [&defaultLogLevel](const PacBio::CLI_v2::Results& results)
    {
        EXPECT_TRUE(results.LogFile().empty());
        EXPECT_EQ(defaultLogLevel, results.LogLevel());
        return EXIT_SUCCESS;
    };

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_fetch_overriden_default_log_level_from_results)
{
    const PacBio::CLI_v2::Option MaxNumLines
    {
    R"({
        "names" : ["n"],
        "description" : "Max Number of lines to Copy",
        "type" : "int",
        "default" : 10,
        "tool_contract.option" : {
            "title" : "Maximum Line Count",
            "id" : "max_nlines"
        }
    })"
    };

    const std::vector<std::string> args {
        "frobber", "-n", "27"
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.AddOption(MaxNumLines);
    const auto defaultLogLevel = PacBio::Logging::LogLevel::DEBUG;
    i.DefaultLogLevel(defaultLogLevel);

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    auto runner = [&MaxNumLines, &defaultLogLevel](const PacBio::CLI_v2::Results& results)
    {
        const int expectedNumLines = 27;
        const int maxNumLines = results[MaxNumLines];

        EXPECT_EQ(expectedNumLines, maxNumLines);
        EXPECT_TRUE(results.LogFile().empty());
        EXPECT_EQ(defaultLogLevel, results.LogLevel());
        return EXIT_SUCCESS;
    };

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_fetch_log_level_from_results)
{
    const std::vector<std::string> args {
        "frobber", "--log-level", "DEBUG"
    };
    auto runner = [](const PacBio::CLI_v2::Results& results)
    {
        EXPECT_TRUE(results.LogFile().empty());
        EXPECT_EQ(PacBio::Logging::LogLevel::DEBUG, results.LogLevel());
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_fetch_log_file_from_results)
{
    const std::vector<std::string> args {
        "frobber", "--log-file", "log.txt"
    };
    auto runner = [](const PacBio::CLI_v2::Results& results)
    {
        EXPECT_EQ("log.txt", results.LogFile());
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, verbosity_is_true_if_enabled_and_user_requested)
{
    const std::vector<std::string> args {
        "frobber", "-v"
    };
    auto runner = [](const PacBio::CLI_v2::Results& results)
    {
        EXPECT_TRUE(results.Verbose());
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.EnableVerboseOption();

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, verbosity_is_false_if_enabled_but_not_user_requested)
{
    const std::vector<std::string> args {
        "frobber"
    };
    auto runner = [](const PacBio::CLI_v2::Results& results)
    {
        EXPECT_FALSE(results.Verbose());
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.EnableVerboseOption();

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, verbosity_is_false_if_not_enabled)
{
    const std::vector<std::string> args {
        "frobber"
    };
    auto runner = [](const PacBio::CLI_v2::Results& results)
    {
        EXPECT_FALSE(results.Verbose());
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_run_tool_with_optional_pos_args)
{
    const std::vector<std::string> args {"frobber"};
    auto runner = [](const PacBio::CLI_v2::Results& results)
    {
        EXPECT_TRUE(results.PositionalArguments().empty());
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.AddPositionalArgument(CLI_v2_CLITests::Source);

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    const int result = PacBio::CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, no_throw_on_disabled_log_file_option_in_setup)
{
    const std::vector<std::string> args {"frobber"};
    auto runner = [](const PacBio::CLI_v2::Results&)
    {
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.DisableLogFileOption();

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    EXPECT_NO_THROW(PacBio::CLI_v2::Run(args, i, runner););
}

TEST(CLI2_CLI, no_throw_on_disabled_log_level_option_in_setup)
{
    const std::vector<std::string> args {"frobber"};
    auto runner = [](const PacBio::CLI_v2::Results&)
    {
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.DisableLogLevelOption();

    std::ostringstream s;
    PacBio::Utility::CoutRedirect redirect{s.rdbuf()};
    std::ignore = redirect;

    EXPECT_NO_THROW(PacBio::CLI_v2::Run(args, i, runner););
}

TEST(CLI2_CLI, multitoolinterface_can_override_subtool_log_config)
{
    auto refineRunner = [](const PacBio::CLI_v2::Results& results) {
        EXPECT_EQ(PacBio::Logging::LogLevel::DEBUG, results.LogLevel());
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::MultiToolInterface i{"isoseq3", "De Novo Transcript Reconstruction", "3.1.2"};
    const auto& refineInterface = CLI_v2_CLITests::MakeRefineInterface();
    i.AddTools({{"refine",    refineInterface,  refineRunner},
                {"cluster",   CLI_v2_CLITests::MakeClusterInterface(),   &CLI_v2_CLITests::ClusterRunner},
                {"polish",    CLI_v2_CLITests::MakePolishInterface(),    &CLI_v2_CLITests::PolishRunner},
                {"summarize", CLI_v2_CLITests::MakeSummarizeInterface(), &CLI_v2_CLITests::SummarizeRunner}});

    PacBio::Logging::LogConfig config{PacBio::Logging::LogLevel::DEBUG};
    i.LogConfig(config);

    EXPECT_EQ(PacBio::Logging::LogLevel::WARN, refineInterface.DefaultLogLevel());
    EXPECT_EQ(PacBio::Logging::LogLevel::DEBUG, i.LogConfig()->Level);

    const std::vector<std::string> args {
        "isoseq3", "refine", "--test", "--count", "10"
    };
    const int result = PacBio::CLI_v2::Run(args, i);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, command_line_log_level_overrides_interface_configs)
{
    auto refineRunner = [](const PacBio::CLI_v2::Results& results) {
        EXPECT_EQ(PacBio::Logging::LogLevel::TRACE, results.LogLevel());
        return EXIT_SUCCESS;
    };

    PacBio::CLI_v2::MultiToolInterface i{"isoseq3", "De Novo Transcript Reconstruction", "3.1.2"};
    const auto& refineInterface = CLI_v2_CLITests::MakeRefineInterface();
    i.AddTools({{"refine",    refineInterface,  refineRunner},
                {"cluster",   CLI_v2_CLITests::MakeClusterInterface(),   &CLI_v2_CLITests::ClusterRunner},
                {"polish",    CLI_v2_CLITests::MakePolishInterface(),    &CLI_v2_CLITests::PolishRunner},
                {"summarize", CLI_v2_CLITests::MakeSummarizeInterface(), &CLI_v2_CLITests::SummarizeRunner}});

    PacBio::Logging::LogConfig config{PacBio::Logging::LogLevel::DEBUG};
    i.LogConfig(config);

    EXPECT_EQ(PacBio::Logging::LogLevel::WARN, refineInterface.DefaultLogLevel());
    EXPECT_EQ(PacBio::Logging::LogLevel::DEBUG, i.LogConfig()->Level);

    const std::vector<std::string> args {
        "isoseq3", "refine", "--test", "--count", "10", "--log-level", "TRACE"
    };
    const int result = PacBio::CLI_v2::Run(args, i);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

// TEST(CLI2_CLI, throws_if_missing_required_number_of_pos_args)
// {
//     const PacBio::CLI_v2::PositionalArgument In {
//     R"({
//         "name" : "in",
//         "description" : "Input file.",
//         "type" : "file"
//     })"
//     };

//     const PacBio::CLI_v2::PositionalArgument Out {
//     R"({
//         "name" : "out",
//         "description" : "Output file.",
//         "type" : "file"
//     })"
//     };

//     const PacBio::CLI_v2::PositionalArgument Report {
//     R"({
//         "name" : "report",
//         "description" : "Report file.",
//         "type" : "file"
//     })"
//     };

//     auto runner = [](const PacBio::CLI_v2::Results& results) {
//         std::ignore = results;
//         return EXIT_SUCCESS;
//     };

//     PacBio::CLI_v2::Interface i {
//         "frobber",
//         "Frobb your files in a most delightful, nobbly way",
//         "3.14"
//     };
//     i.AddPositionalArguments({In, Out, Report});

//     try {
//         const std::vector<std::string> args {"frobber", "in.file"};
//         const auto result = PacBio::CLI_v2::Run(args, i, runner);

//         std::ignore = result;
//         ASSERT_FALSE("should not reach here");
//     } catch (std::runtime_error& e) {
//         const std::string msg{
//             "[pbcopper] command line parser ERROR: requires 3 arguments, but 1 was provided\n"
//         };
//         EXPECT_EQ(msg, e.what());
//     }
//     try {
//         const std::vector<std::string> args {"frobber", "in.file", "out.file"};
//         const auto result = PacBio::CLI_v2::Run(args, i, runner);

//         std::ignore = result;
//         ASSERT_FALSE("should not reach here");
//     } catch (std::runtime_error& e) {
//         const std::string msg{
//             "[pbcopper] command line parser ERROR: requires 3 arguments, but 2 were provided\n"
//         };
//         EXPECT_EQ(msg, e.what());
//     }
// }

// clang-format on
